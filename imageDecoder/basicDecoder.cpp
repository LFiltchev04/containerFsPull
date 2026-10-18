#include "basicDecoder.hpp"


 #pragma pack(push, 1)
    struct packedRecord{
        uint8_t xxHashID[6];
        uint64_t fArchOffset;
        mode_t fileType;
        std::string recordName;
        uint16_t trimSize;
    };
#pragma pack(pop)


basicDecoder::basicDecoder(std::string hash){

    //just opens the base archive
    archPath = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash;
    
    std::string temp = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash + "/index.offst";

}