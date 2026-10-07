#include "basicDecoder.hpp"


basicDecoder::basicDecoder(std::string hash){


    globalBackendPV += "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash + "/index.offst";

}