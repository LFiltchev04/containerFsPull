#pragma once
#include <liburing.h>
#include <mutex>
#include <stdexcept>
#include "../../utils/uringHandler.hpp"



void cqeFinish(struct io_uring_cqe* cqe){
    resumeCtx* ctxW = (resumeCtx*)(cqe->user_data & ~STAGE_MASK);
    unsigned long stage = cqe->user_data & STAGE_MASK;
    if(!ctxW) return;

    // header and file-to-pipe completions only matter on failure; the socket splice drives progress
    if(stage != STAGE_PIPE_TO_SOCK){
        if(cqe->res < 0 && cqe->res != -ECANCELED){
            throw std::runtime_error("uring submission failed");
        }
        return;
    }

    bool done = false;
    {
        std::lock_guard<std::mutex> lock(ctxW->mtx);
        if(cqe->res < 0){
            throw std::runtime_error("uring socket splice failed");
        }
        ctxW->targetWrite -= cqe->res;
        ctxW->offsetTrack += cqe->res;
        done = (ctxW->targetWrite == 0);
        if(!done){
            prepUring(ctxW);
        }
    }
    if(done){
        ctxW->headerSent = false;
        resumeCtxPool->yield(ctxW); // pool-owned, never delete
    }
}
