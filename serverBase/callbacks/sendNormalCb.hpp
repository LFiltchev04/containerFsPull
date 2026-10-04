#pragma once

#include <unistd.h>

#include <nghttp2/nghttp2.h>
#include "../sockCtx.hpp"

long sendNormalCb(nghttp2_session *session, const uint8_t *data, size_t length, int flags, void *user_data){
    printf("sendNormalCb called with length: %zu\n", length);
    ssize_t written = write(static_cast<socketCtx*>(user_data)->outgoingFd, data, length);
    if (written == -1) {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    return static_cast<long>(written);
}
