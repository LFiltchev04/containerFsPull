#include <nghttp2/nghttp2.h>




int dataChunkWriter(nghttp2_session *session, 
        uint8_t flags, int32_t stream_id, 
        const uint8_t *data, size_t len, 
        void *user_data){


        auto ctx = static_cast<>(user_data);
        return 0;
        }