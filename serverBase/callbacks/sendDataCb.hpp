#pragma once

#include <nghttp2/nghttp2.h>
#include <cstring>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"
#include "../sockCtx.hpp"

int sendDataCb(nghttp2_session *session,
    nghttp2_frame *frame, 
    const uint8_t *framehd, 
    size_t length, 
    nghttp2_data_source *source, 
    void *user_data){

        if(frame->hd.type != NGHTTP2_DATA){
            //just the OK, sync writer

            printf("Non-DATA frame encountered\n");
        }

        printf("Processing DATA frame\n");

        auto* tmp = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
        reqLifecycleCtx* ctx = (reqLifecycleCtx*)tmp;

        if(ctx == nullptr){
            return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
        }

        if(sizeof(ctx->write->frameHeader) > length){
            throw std::runtime_error("frame header size mismatch to buffer");
        }
        
        // nghttp2 emits one DATA frame per chunk, in order, subheads are added because uring can reorder them
        memcpy((void*)framehd, ctx->write->frameHeader, sizeof(ctx->write->frameHeader));
        prepUring(ctx->write);

        

        return 0;
    }