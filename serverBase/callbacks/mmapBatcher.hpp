#pragma once
#include <cstdint>
#include <sys/uio.h>

struct batchRange{
    uint64_t fileHashes[64]; //holds xxHashes for the requested files, context derived 
    
    //makes sure to return the byte offsets to an invalid state as they are reusable
    void zeroOut(){
        for(int x = 0; x < 64; x++){
            fileHashes[x] = UINT64_MAX;
        }
    }
};


// fires off a VMA backed iovec for all assigned batches
void mmapBatchPrep(void* mmapAddr, batchRange* range){
    
    iovec iov[64];
    for(int x = 0; x < 64; x++){
        if(range->fileHashes[x] == UINT64_MAX){ //i truncate the top bits so the top 2 orders of magnitude are guaranteed to not have anything
            continue;
        }



    }
}