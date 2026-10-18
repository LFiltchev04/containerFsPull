#pragma once
#include <liburing.h>
#include <sys/uio.h>

#include "batchWriteCtx.hpp"



void handleBatchCqe(io_uring_cqe* cqe){
    auto ctx = (batchWriteCtx*)cqe->user_data;
    
    uint32_t totalWriteSize = 0;
    for(int i = 0; i < ctx->filled; ++i){
        if(ctx->iov[i].iov_len > 0){
            totalWriteSize += ctx->iov[i].iov_len;
        }
    }


    
}