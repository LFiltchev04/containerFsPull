#include <liburing.h>
#include <stack>
#include <cstring>
#include <cstdint>
#include <sys/uio.h>
#include <mutex>

#include "genericStatAlloc.hpp"
#include "subheaderStruct.hpp"
io_uring ring;

struct resumeCtx{
    std::mutex mtx;

    int *fd;
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];
    uint8_t frameHeader[9]; // serialized htttp2 frame header


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


void prepUring(resumeCtx* ctxW){

    if(ring.flags == IORING_SQ_NEED_WAKEUP){
        io_uring_enter(ring.ring_fd, 0, 0, IORING_ENTER_SQ_WAKEUP, nullptr);
    }

    subheader sh;
    sh.length = ctxW->targetWrite;
    sh.offset = ctxW->offsetTrack;

    iovec iov[2] = {
        {&sh, sizeof(sh)},
        {ctxW->frameHeader, sizeof(ctxW->frameHeader)}
    };

    io_uring_sqe *sqeH = io_uring_get_sqe(&ring);
    io_uring_prep_writev(sqeH, *ctxW->fd, iov, 2, ctxW->offsetTrack);
    sqeH->user_data = (unsigned long)ctxW;

    //file-to-pipe
    io_uring_sqe *sqeD = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeD, ctxW->pipes[0], ctxW->offsetTrack, ctxW->pipes[1], ctxW->offsetTrack, ctxW->targetWrite, SPLICE_F_MORE);
    sqeD->flags = IOSQE_IO_LINK;
    sqeD->user_data = (unsigned long)ctxW;

    //pipe to socket
    io_uring_sqe *sqeN = io_uring_get_sqe(&ring);
    io_uring_prep_splice(sqeN, ctxW->pipes[1], ctxW->offsetTrack, *ctxW->fd, ctxW->offsetTrack, ctxW->targetWrite, SPLICE_F_MORE);
    sqeN->user_data = (unsigned long)ctxW;

    io_uring_submit(&ring);
}
