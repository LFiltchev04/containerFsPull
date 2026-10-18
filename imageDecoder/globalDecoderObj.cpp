#include "globalDecoderObj.hpp"


globalDecoderObj::globalDecoderObj() {}


//reads type, saves it to seen hashes
void globalDecoderObj::addDecoder(std::string hash){

    std::string baseLocation = GLOBAL_BACKEND_PV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash;
    std::string properLocation = baseLocation+"/meta";  //the data symlink, bulk tar is here
    
    int fd = open(properLocation.c_str(), O_RDONLY);

    if (fd < 0) {
        printf("couldnt open file: %s\n", properLocation.c_str());
        return;
    }
    
    uint8_t typeBuf;
    read(fd, &typeBuf, 1);
    close(fd);

    if(typeBuf == PLAIN_TAR){
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


