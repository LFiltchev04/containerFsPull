#pragma once
#include <liburing.h>
#include <sys/uio.h>

#include "uringHandler.hpp"



void handleBatchCqe(io_uring_cqe* cqe){
    auto ctx = (batchWriteCtx*)cqe->user_data;
    
    //have to calculate this per run, figured its easier
    uint32_t totalWriteSize = 0;
    int highestIov = 0;
    int iovRemainder = 0;
    for(int i = 0; i < ctx->filled; ++i){
        if(ctx->iov[i].iov_len > 0){
            totalWriteSize += ctx->iov[i].iov_len;
            
            //check for shorting
            if(totalWriteSize > cqe->res){
                highestIov = i;
                iovRemainder = cqe->res - (totalWriteSize - ctx->iov[highestIov].iov_len); //handles the remainder of teh iovec
                break;
            }

            //means it completed fine, just return buffer
            if(totalWriteSize == cqe->res){
                batchWriteCtxPool->yield(ctx);
                return;
            }
        }
        //partial resubmit for separate iovec entries
        
        //handles the partial iovec
        ctx->iov[0].iov_base = (char*)ctx->iov[highestIov].iov_base + iovRemainder;
        ctx->iov[0].iov_len = ctx->iov[highestIov].iov_len - iovRemainder;

        //pushes the remaining iovec entries to the front
        for(int i = highestIov + 1; i < ctx->filled; ++i){
            ctx->iov[i - highestIov].iov_base = ctx->iov[i].iov_base;
            ctx->iov[i - highestIov].iov_len = ctx->iov[i].iov_len;
        }

        ctx->filled -= highestIov;
    }


}