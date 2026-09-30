#include <string_view>

#include <nghttp2/nghttp2.h>


#define MAKE_NV(NAME, VALUE) \
    { (uint8_t *)(NAME), (uint8_t *)(VALUE), sizeof(NAME) - 1, sizeof(VALUE) - 1, NGHTTP2_NV_FLAG_NONE }


int onFrameRecv(nghttp2_session *session, const nghttp2_frame *frame, void *user_data){

    
    if (frame->hd.type == NGHTTP2_HEADERS && frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
        nghttp2_nv hdrs[] = {
            MAKE_NV(":status", "200")
            //MAKE_NV("content-type", "application/octet-stream")
        };
    }

    nghttp2_data_provider provider;
    

    return 0;
}