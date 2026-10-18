#include <sys/uio.h>


struct batchWriteCtx{
    int outgoingFd = -1;
    int filled = -1;
    
    int targetWrite = -1;
    iovec iov[64];
};