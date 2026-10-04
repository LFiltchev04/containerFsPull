#include <liburing.h>
#include <stack>
#include <cstring>
#include <cstdint>

#include "genericStatAlloc.hpp"
io_uring ring;

struct resumeCtx{
    int *fd;
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];
    uint8_t frameHeader[9]; // serialized HTTP/2 frame header


    resumeCtx(){
        fd = nullptr;
        offsetTrack = 0;
        targetWrite = 0;
        pipes[0] = -1;
        pipes[1] = -1;
        memset(frameHeader, 0, sizeof(frameHeader));
    }
};



//thread safe entry for submissions
staticAllocatedPool<resumeCtx>* resumeCtxPool;
void setupUring(){
    struct io_uring_params params = {0};
    params.flags = IORING_SETUP_SQPOLL;
    params.sq_thread_idle = 600;
    io_uring_queue_init_params(8192, &ring, &params);


    resumeCtxPool = new staticAllocatedPool<resumeCtx>(8192);
    
}


void prepUring(resumeCtx* ctx){

    if(ring.flags == IORING_SQ_NEED_WAKEUP){
        io_uring_enter(ring.ring_fd, 0, 0, IORING_ENTER_SQ_WAKEUP, nullptr);
    }

    //file-to-pipe
    io_uring_sqe *sqeD = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeD, ctx->pipes[0], ctx->offsetTrack, ctx->pipes[1], ctx->offsetTrack, ctx->targetWrite, SPLICE_F_MORE);
    sqeD->flags = IOSQE_IO_LINK;
    sqeD->user_data = (unsigned long)ctx;

    //pipe to socket
    io_uring_sqe *sqeN = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeN, ctx->pipes[1], ctx->offsetTrack, *ctx->fd, ctx->offsetTrack, ctx->targetWrite, SPLICE_F_MORE);
    sqeN->user_data = (unsigned long)ctx;

    io_uring_submit(&ring);
}

