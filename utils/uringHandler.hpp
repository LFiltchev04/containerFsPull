#include <liburing.h>
#include <stack>

#include "genericStatAlloc.hpp"
io_uring ring;

struct resumeCtx{
    int *fd;
    uint32_t targetWrite;
    int pipes[2];

    resumeCtx(){
        fd = nullptr;
        targetWrite = 0;
        pipes[0] = -1;
        pipes[1] = -1;
    }
};

//thread safe entry for submissions
staticAllocatedPool<resumeCtx>* resumeCtxPool;
void setupUring(){
    io_uring_queue_init(8192, &ring, 0);
    resumeCtxPool = new staticAllocatedPool<resumeCtx>(8192);
}


