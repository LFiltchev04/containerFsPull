#pragma once
#include <liburing.h>
#include <mutex>
#include <stdexcept>
#include "../../utils/uringHandler.hpp"



void cqeFinish(struct io_uring_cqe* cqe){


    if(!cqe){
        return;
    }

    resumeCtx* ctxW = (resumeCtx*)cqe->user_data;
    std::lock_guard<std::mutex> lock(ctxW->mtx);
    if(cqe->res < 0){
        throw std::runtime_error("uring socket splice failed");
    }

    if(ctxW->stage == PIPE_TO_SOCK){
        printf("writeCOmpletionCallback? \n");

        ctxW->targetWrite -= cqe->res;
        ctxW->offsetTrack += cqe->res;

        if(ctxW->targetWrite == 0){
            ctxW->stage = COMPLETE;
        }
    }else{
        ctxW->subheaderOffset += cqe->res;
        if(ctxW->subheaderOffset >= sizeof(ctxW->sh)){
            ctxW->stage = PIPE_TO_SOCK;
        }
    }
    if(ctxW->stage == COMPLETE){
        //call the global cleanup, dump the stream CTX, return everything to pool

        resumeCtxPool->yield(ctxW);

        //wire in nghttp2 streamID dump, if you dont clear them out you hit max connections, they dont resolve alone, maybe send a termination
        //so the client clears out any stream scoped ctx it had
    }else{
        printf("epoll errored out, errorL %s\n", strerror(-cqe->res));
    }
    
}
