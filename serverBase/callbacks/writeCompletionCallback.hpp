#pragma once
#include <liburing.h>
#include <mutex>
#include "../../utils/uringHandler.hpp"



void cqeFinish(struct io_uring_cqe* cqe){
    resumeCtx* ctxW = (resumeCtx*)cqe->user_data;
    if(ctxW){
        std::lock_guard<std::mutex> lock(ctxW->mtx);
        if(cqe->res < 0){
            throw std::exception();
        }else{
            ctxW->targetWrite -= cqe->res;
            ctxW->offsetTrack += cqe->res;

            if(ctxW->targetWrite == 0){
                delete ctxW;
            }else{
                prepUring(ctxW);
            }
        }
    }
}


