#pragma once

#include <sys/uio.h>



struct batchWriteCtx{
    int outgoingFd = -1;
    int filled = -1;

    
    int targetWrite = -1;
    iovec iov[64];

    void init(){
        outgoingFd = -1;
        filled = -1;
        targetWrite = -1;
        for(int i = 0; i < 64; ++i){
            iov[i].iov_base = nullptr;
            iov[i].iov_len = 0;
        }
    }

    void reset(){
        outgoingFd = -1;
        filled = -1;
        targetWrite = -1;
        for(int i = 0; i < 64; ++i){
            iov[i].iov_base = nullptr;
            iov[i].iov_len = 0;
        }
    }
};