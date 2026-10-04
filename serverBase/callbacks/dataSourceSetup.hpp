#pragma once

#include <string_view>

#include <nghttp2/nghttp2.h>

#include "datasourceCallback.hpp"

#define MAKE_NV(NAME, VALUE) \
    { (uint8_t *)(NAME), (uint8_t *)(VALUE), sizeof(NAME) - 1, sizeof(VALUE) - 1, NGHTTP2_NV_FLAG_NONE }


int onFrameRecv(nghttp2_session *session, const nghttp2_frame *frame, void *user_data){
    printf("frame received: type=%d\n", frame->hd.type);
    

    if(frame->hd.type == NGHTTP2_HEADERS && frame->headers.cat == NGHTTP2_HCAT_REQUEST){
        printf("Received request headers for stream_id=%d\n", frame->hd.stream_id);
    
        nghttp2_nv hdrs[2];
        if (frame->hd.type == NGHTTP2_HEADERS && frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
            hdrs[0] = MAKE_NV(":status", "200");
            hdrs[1] = MAKE_NV("content-type", "application/octet-stream");
        }



        //nghttp2_data_provider provider;
        //provider.read_callback = dataReadCb;
    
        printf("Submitting response for stream_id=%d\n", frame->hd.stream_id);

        if(frame->hd.stream_id % 2 != 0){
            int res = nghttp2_submit_response(session, frame->hd.stream_id, hdrs, 2, nullptr);
            printf("nghttp2_submit_response returned: %d\n", res);
            nghttp2_session_send(session);
        
        }
    }
    
    return 0;
}