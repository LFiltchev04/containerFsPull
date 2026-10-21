#pragma once

#include <nghttp2/nghttp2.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <unistd.h>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"
#include "../sockCtx.hpp"


constexpr size_t kHttp2FrameHeaderLen = 9;
uint8_t sixtyFourKilobytes[65536];

// blocking-style write of the whole buffer, tolerates EINTR and non-blocking sockets
inline bool writeAll(int fd, const void* data, size_t len){
    const uint8_t* p = static_cast<const uint8_t*>(data);
    while(len > 0){
        ssize_t n = write(fd, p, len);
        if(n < 0){
            if(errno == EINTR) continue;
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                pollfd pfd{fd, POLLOUT, 0};
                poll(&pfd, 1, -1);
                continue;
            }
            return false;
        }
        p += n;
        len -= static_cast<size_t>(n);
    }
    return true;
}

int sendDataCb(nghttp2_session *session,
    nghttp2_frame *frame, 
    const uint8_t *framehd, 
    size_t length, 
    nghttp2_data_source *source,
    void *user_data){

        auto* tmp = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
        reqLifecycleCtx* ctx = (reqLifecycleCtx*)tmp;

        if(ctx == nullptr || ctx->write == nullptr || length > sizeof(sixtyFourKilobytes)){
            return NGHTTP2_ERR_CALLBACK_FAILURE;
        }

        ctx->write->networkFd = static_cast<socketCtx*>(user_data)->outgoingFd;

        // read first so a failed read never leaves a half-written frame on the wire
        size_t got = 0;
        while(got < length){
            ssize_t n = pread(ctx->openFd, sixtyFourKilobytes + got, length - got, ctx->write->offsetTrack + got);
            if(n < 0 && errno == EINTR) continue;
            if(n <= 0) return NGHTTP2_ERR_CALLBACK_FAILURE;
            got += static_cast<size_t>(n);
        }

        ctx->write->sh.length = static_cast<decltype(ctx->write->sh.length)>(length);
        ctx->write->sh.offset = static_cast<decltype(ctx->write->sh.offset)>(ctx->write->offsetTrack);

        // framehd is a pointer, the HTTP/2 frame header is always 9 bytes
        if(!writeAll(ctx->write->networkFd, framehd, kHttp2FrameHeaderLen)
            || !writeAll(ctx->write->networkFd, &ctx->write->sh, sizeof(ctx->write->sh))
            || (length > 0 && !writeAll(ctx->write->networkFd, sixtyFourKilobytes, length))){
            return NGHTTP2_ERR_CALLBACK_FAILURE;
        }

        printf("Sent DATA frame of length: %zu\n", length);
        ctx->write->offsetTrack += length;
        return 0;
    }
