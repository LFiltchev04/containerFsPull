#pragma once
#include <cstdint>
#include <sys/uio.h>
#include <liburing.h>

#include "../../imageDecoder/globalDecoderObj.hpp"
#include "../../imageDecoder/basicDecoder.hpp"



//DUMMY GLOBAL DECODER, GET RID OF IT FOR A REAL CONTEXT

globalDecoderObj decode;

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
void mmapBatchPrep(void* mmapAddr, batchRange* range, std::string &imageHash, int outgoingFd){
    
    iovec iov[64];
    int filled = 0;
    for(int x = 0; x < 64; x++){
        if(range->fileHashes[x] == UINT64_MAX){ //i truncate the top bits so the top 2 orders of magnitude are guaranteed to not have anything
            continue;
        }
        filled++;

        basicDecoder* decoder = decode.getDecoder(imageHash);
        xxHashStruct xxHstruct = decoder->fromHashPath(range->fileHashes[x]);
        
        iov[x].iov_base = mmapAddr + xxHstruct.fileOffset;
        iov[x].iov_len = xxHstruct.fileSize;
    }

    io_uring_prep_writev(&sqe, outgoingFd, iov, filled, 0);
    writev(datafileFd, iov, filled);
}