#pragma once


#include <liburing.h>
#include <sys/stat.h>
#include <vector>

#include <nghttp2/nghttp2.h>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"
int chunkLimit = 16384;




ssize_t dataReadCb(nghttp2_session *session, 
    int32_t stream_id,
    uint8_t *buf, size_t length,
    uint32_t *data_flags,
    nghttp2_data_source *source,
    void *user_data){
    printf("dataReadCb called for stream_id: %d\n", stream_id);

    *data_flags |= NGHTTP2_DATA_FLAG_NO_COPY;

    auto tmp = nghttp2_session_get_stream_user_data(session, stream_id);
    auto ctx = static_cast<reqLifecycleCtx *>(tmp);

    struct stat st;
    stat(ctx->path.c_str(), &st);

    printf("File size: %ld\n", st.st_size);

    uint64_t fileSize = static_cast<uint64_t>(st.st_size);
    if (ctx->fileOffset >= fileSize) {
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
        return 0;
    }

    uint64_t remaining = fileSize - ctx->fileOffset;
    size_t bytesToSend = length;
    if (bytesToSend > static_cast<size_t>(chunkLimit)) {
        bytesToSend = static_cast<size_t>(chunkLimit);
    }
    if (remaining < bytesToSend) {
        bytesToSend = static_cast<size_t>(remaining);
    }

    if (ctx->write == nullptr) {
        ctx->write = resumeCtxPool->get();
    }

    ctx->write->targetWrite = static_cast<uint32_t>(bytesToSend);
    ctx->write->offsetTrack = static_cast<uint32_t>(ctx->fileOffset);
    ctx->write->fileFd = ctx->openFd;

    ctx->fileOffset += bytesToSend;
    if (ctx->fileOffset == fileSize) {
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
    }

    return static_cast<ssize_t>(bytesToSend);
}