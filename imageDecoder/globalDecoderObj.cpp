#include "globalDecoderObj.hpp"



globalDecoderObj::globalDecoderObj() {}


void globalDecoderObj::addDecoder(std::string location){
    int fd = open(location.c_str(), O_RDONLY);

    if (fd < 0) {
        printf("couldnt open file: %s\n", location.c_str());
        return;
    }


    int fd = open(location.c_str(), O_RDONLY);
    if(fd < 0){
        printf("couldnt open file: %s\n", location.c_str());
        return;
    }

    char typeBuf[16];
    read(fd, typeBuf, 16);
    close(fd);
    uint16_t type = *(uint16_t*)typeBuf;

    if(type == PLAIN_TAR){
        decoders.push_back(new tarDecoder(location));
    }
}