#pragma once

#include <liburing.h>
#include <stack>
#include <cstring>
#include <cstdint>
#include <sys/uio.h>
#include <mutex>
#include <cerrno>

#include "genericStatAlloc.hpp"
#include "subheaderStruct.hpp"
inline io_uring ring;
inline std::mutex ringMtx; 


// stage tag stored in the low bits of user_data (ctx is 8-byte aligned)
enum writeState{ 
    HEADER = 0, //if header is sent the initial iovec must be skipped
    PIPE_TO_SOCK = 10, // the file->pipe operation is carefully sized and is thus expected to never actually come short, only the network socket is a problem
    COMPLETE = 20, // write completed, not even sure i need this thing
};


struct resumeCtx{
    inline static int devNullFd;
    std::mutex mtx;

    int *networkFd = nullptr;  
    int *fileFd = nullptr;
    subheader sh;
    uint8_t subheaderOffset = 0u;
    iovec iov[2];
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];
    uint8_t frameHeader[9]; // serialized htttp2 frame header

    writeState stage = HEADER;

    resumeCtx(){
        networkFd = nullptr;
        offsetTrack = 0;
        targetWrite = 0;

        if(devNullFd == 0){
            devNullFd = open("/dev/null", O_WRONLY);
        }

        pipes[0] = -1;
        pipes[1] = -1;
        memset(frameHeader, 0, sizeof(frameHeader));
    }

    void reset(){
        networkFd = nullptr;
        fileFd = nullptr;
        offsetTrack = 0;
        targetWrite = 0;
        subheaderOffset = 0u;

        if(pipes[0] != -1 || pipes[1] != -1){
            write(devNullFd, &pipes[1], 1);
        }

    }
};



//thread safe entry for submissions
extern staticAllocatedPool<resumeCtx>* resumeCtxPool;
void setupUring(){
    struct io_uring_params params = {0};
    params.flags = IORING_SETUP_SQPOLL;
    params.sq_thread_idle = 600;
    io_uring_queue_init_params(8192, &ring, &params);


    resumeCtxPool = new staticAllocatedPool<resumeCtx>(8192);
    
}

//idempotent function for resubmission of uring events, context knows how to recover each short-state
void prepUring(resumeCtx* ctxW){
    std::lock_guard<std::mutex> lock(ringMtx);

    // sofit links work, figured it sbetter to yield the sqe entries early and to make as small of an operation as possible, maybe easier on worker threads to schedule
    if(ctxW->stage == HEADER){
        ctxW->sh.length = ctxW->targetWrite;
        ctxW->sh.offset = ctxW->offsetTrack;
        ctxW->iov[0] = { &ctxW->sh, sizeof(ctxW->sh) };
        ctxW->iov[1] = { ctxW->frameHeader, sizeof(ctxW->frameHeader) };

        io_uring_sqe *sqeH = io_uring_get_sqe(&ring);
        io_uring_prep_writev(sqeH, *ctxW->networkFd, ctxW->iov, 2, ctxW->subheaderOffset);
        sqeH->flags = IOSQE_IO_LINK;

        sqeH->user_data = (unsigned long)ctxW;
    }

    //file-to-pipe
    io_uring_sqe *sqeD = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeD, *ctxW->fileFd, ctxW->offsetTrack, ctxW->pipes[1], -1, ctxW->targetWrite, 0);
    sqeD->flags = IOSQE_IO_LINK;
    sqeD->user_data = (unsigned long)ctxW;

    //pipe to socket
    io_uring_sqe *sqeN = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeN, ctxW->pipes[0], -1, *ctxW->networkFd, -1, ctxW->targetWrite, 0);
    sqeN->user_data = (unsigned long)ctxW;
    //there was no need to this? Its the end of the logical chain?

    io_uring_submit(&ring);
}
