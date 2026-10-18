#include "globalDecoderObj.hpp"


globalDecoderObj::globalDecoderObj() {}


void globalDecoderObj::addDecoder(std::string hash){

    std::string properLocation = GLOBAL_BACKEND_PV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash;
    int fd = open(properLocation.c_str(), O_RDONLY);

    if (fd < 0) {
        printf("couldnt open file: %s\n", properLocation.c_str());
        return;
    }


    int fd = open(properLocation.c_str(), O_RDONLY);
    if(fd < 0){
        printf("couldnt open file: %s\n", properLocation.c_str());
        return;
    }

    char typeBuf[16];
    read(fd, typeBuf, 16);
    close(fd);
    uint16_t type = *(uint16_t*)typeBuf;

    if(type == PLAIN_TAR){
        decoders.push_back(new tarDecoder(properLocation));
    }

    this->seenHashes[hash] = decoders.back();
}


basicDecoder* globalDecoderObj::getDecoder(std::string hash){
    if(seenHashes.find(hash) != seenHashes.end()){
        return seenHashes[hash];
    }

    printf("tried to run a hash, got nothing: %s\n", hash.c_str());
    return nullptr;
}


