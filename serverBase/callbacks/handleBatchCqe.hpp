#pragma once
#include <liburing.h>
#include <sys/uio.h>

#include "uringHandler.hpp"



void handleBatchCqe(io_uring_cqe* cqe){
    auto ctx = (batchWriteCtx*)cqe->user_data;
    
    //have to calculate this per run, figured its easier
    uint32_t totalWriteSize = 0;
    int highestIov = 0;
    for(int i = 0; i < ctx->filled; ++i){
        if(ctx->iov[i].iov_len > 0){
            totalWriteSize += ctx->iov[i].iov_len;
            
            //check for shorting
            if(totalWriteSize > cqe->res){
                highestIov = i;
                break;
            }

            //means it completed fine, just return buffer
            if(totalWriteSize == cqe->res){
                batchWriteCtxPool->yield(ctx);
                return;
            }
        }
    }


}