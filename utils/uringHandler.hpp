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
enum : unsigned long { STAGE_HEADER = 0, STAGE_FILE_TO_PIPE = 1, STAGE_PIPE_TO_SOCK = 2, STAGE_MASK = 7 };


struct resumeCtx{
    static int devNullFd;
    std::mutex mtx;

    int *fd;          // socket
    int fileFd = -1;  // source file
    bool headerSent = false;
    subheader sh;     // must outlive submission
    iovec iov[2];     // must outlive submission
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];
    uint8_t frameHeader[9]; // serialized htttp2 frame header


    resumeCtx(){
        fd = nullptr;
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
        fd = nullptr;
        fileFd = -1;
        headerSent = false;
        offsetTrack = 0;
        targetWrite = 0;

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


void prepUring(resumeCtx* ctxW){
    std::lock_guard<std::mutex> lock(ringMtx);

    // each step is linked to the next; HARDLINK keeps the chain alive on short transfers
    if(!ctxW->headerSent){
        ctxW->sh.length = ctxW->targetWrite;
        ctxW->sh.offset = ctxW->offsetTrack;
        ctxW->iov[0] = { &ctxW->sh, sizeof(ctxW->sh) };
        ctxW->iov[1] = { ctxW->frameHeader, sizeof(ctxW->frameHeader) };
        ctxW->headerSent = true;

        io_uring_sqe *sqeH = io_uring_get_sqe(&ring);
        io_uring_prep_writev(sqeH, *ctxW->fd, ctxW->iov, 2, 0);
        sqeH->flags = IOSQE_IO_HARDLINK;
        sqeH->user_data = (unsigned long)ctxW | STAGE_HEADER;
    }

    //file-to-pipe
    io_uring_sqe *sqeD = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeD, ctxW->fileFd, ctxW->offsetTrack, ctxW->pipes[1], -1, ctxW->targetWrite, 0);
    sqeD->flags = IOSQE_IO_HARDLINK;
    sqeD->user_data = (unsigned long)ctxW | STAGE_FILE_TO_PIPE;

    //pipe to socket
    io_uring_sqe *sqeN = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeN, ctxW->pipes[0], -1, *ctxW->fd, -1, ctxW->targetWrite, 0);
    sqeN->user_data = (unsigned long)ctxW | STAGE_PIPE_TO_SOCK;

    io_uring_submit(&ring);
}
