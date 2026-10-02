#include <liburing.h>
#include <stack>

#include "genericStatAlloc.hpp"
io_uring ring;

struct resumeCtx{
    int *fd;
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];

    resumeCtx(){
        fd = nullptr;
        offsetTrack = 0;
        targetWrite = 0;
        pipes[0] = -1;
        pipes[1] = -1;
    }
};

void doUring(resumeCtx* ctx){
    io_uring_sqe *sqe = io_uring_get_sqe(&ring);

    io_uring_prep_splice(sqe, ctx->pipes[0], ctx->offsetTrack, ctx->pipes[1], ctx->offsetTrack, ctx->targetWrite, SPLICE_F_MOVE | SPLICE_F_MORE);
}

//thread safe entry for submissions
staticAllocatedPool<resumeCtx>* resumeCtxPool;
void setupUring(){
    io_uring_queue_init(8192, &ring, 0);
    resumeCtxPool = new staticAllocatedPool<resumeCtx>(8192);
}


