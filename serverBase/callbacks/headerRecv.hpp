#pragma once

#include <string_view>

#include <nghttp2/nghttp2.h>

#include "reqLifecycleCtx.hpp"

int onHeaderRecvCb(nghttp2_session *session,
    const nghttp2_frame *frame, 
    const uint8_t *name, 
    size_t namelen, 
    const uint8_t *value, 
    size_t valuelen, 
    uint8_t flags, void *user_data){

    printf("header gotten");
    std::string_view headerName(reinterpret_cast<const char*>(name), namelen);
    std::string_view headerValue(reinterpret_cast<const char*>(value), valuelen);

    if(headerName != ":method" and headerValue != "GET"){
        //-----!kills the entire stream
    }

    void* usrData = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
    if(usrData == nullptr){
        usrData = new reqLifecycleCtx{};
        nghttp2_session_set_stream_user_data(session, frame->hd.stream_id, usrData);
    }

    reqLifecycleCtx* ctx = static_cast<reqLifecycleCtx*>(usrData);
    if(headerName == "pathT"){
        ctx->path = headerValue.data();
        ctx->path.shrink_to_fit(); //alloc and then dealloc, a wonder
    }

    return 0;
}


