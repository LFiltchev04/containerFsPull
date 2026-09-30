#include <string_view>

#include <nghttp2/nghttp2.h>


void onHeaderRecvCb(nghttp2_session *session,
    const nghttp2_frame *frame, 
    const uint8_t *name, 
    size_t namelen, 
    const uint8_t *value, 
    size_t valuelen, 
    uint8_t flags, void *user_data){

    
    std::string_view headerName(reinterpret_cast<const char*>(name), namelen);
    std::string_view headerValue(reinterpret_cast<const char*>(value), valuelen);


}