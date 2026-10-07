#include "basicDecoder.hpp"


basicDecoder::basicDecoder(std::string hash){

    //just opens the base archive
    baseDirPath = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash; 
    std::string temp = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash + "/index.offst"; 
    baseOffsetFd = open(temp.data(), O_RDONLY);
}