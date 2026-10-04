
#include <nghttp2/nghttp2.h>
#include <cstring>

#include "reqLifecycleCtx.hpp"
#include "../../utils/uringHandler.hpp"

int sendDataCb(nghttp2_session *session,
    nghttp2_frame *frame, 
    const uint8_t *framehd, 
    size_t length, 
    nghttp2_data_source *source, 
    void *user_data){

        auto* tmp = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
        reqLifecycleCtx* ctx = (reqLifecycleCtx*)tmp;

        if(ctx == nullptr || ctx->nextFrameCtx >= ctx->uringCtxs.size()){
            return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
        }

        // nghttp2 emits one DATA frame per chunk, in order
        resumeCtx* wrtCtx = ctx->uringCtxs[ctx->nextFrameCtx];
        if(length != wrtCtx->targetWrite){
            return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
        }

        memcpy(wrtCtx->frameHeader, framehd, sizeof(wrtCtx->frameHeader));
        ctx->nextFrameCtx++;

        return 0;
    }