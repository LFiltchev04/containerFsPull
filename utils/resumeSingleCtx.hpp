#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "subheaderStruct.hpp"


enum writeState{ 
    HEADER = 0, //if header is sent the initial iovec must be skipped
    PIPE_TO_SOCK = 10, // the file->pipe operation is carefully sized and is thus expected to never actually come short, only the network socket is a problem
    COMPLETE = 20, // write completed, not even sure i need this thing
};



struct resumeCtx{
    inline static int devNullFd;
    std::mutex mtx;

    int networkFd = -1;  
    int fileFd = -1;
    subheader sh;
    uint8_t subheaderOffset = 0u;
    iovec iov[2];
    uint32_t offsetTrack;
    uint32_t targetWrite;
    int pipes[2];
    uint8_t frameHeader[9]; // serialized htttp2 frame header

    writeState stage = HEADER;

    resumeCtx(){
        networkFd = -1;
        offsetTrack = 0;
        targetWrite = 0;

        if(devNullFd == 0){
            devNullFd = open("/dev/null", O_WRONLY);
        }
       
        memset(frameHeader, 0, sizeof(frameHeader));
    }

    void init(){
        pipe(pipes);
    }
    void reset(){
        networkFd = -1;
        fileFd = -1;
        offsetTrack = 0;
        targetWrite = 0;
        subheaderOffset = 0u;

        if(pipes[0] != -1 || pipes[1] != -1){
            write(devNullFd, &pipes[1], 1);
            close(pipes[0]);
            close(pipes[1]);
            pipes[0] = -1;
            pipes[1] = -1;
        }

    }
};