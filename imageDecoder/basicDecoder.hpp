#pragma once
#include <string>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <fcntl.h>



struct xxHashStruct {
    char xxHashID[8];
    uint16_t recordLen;
    std::string pathName;
};

// Only the trivially-copyable prefix (xxHashID + recordLen) is stored in the mmap slot.
constexpr size_t XX_SLOT_SIZE = offsetof(xxHashStruct, recordLen) + sizeof(uint16_t);

std::string globalBackendPV;
//implements basic functionality for distribution backend traversal opens one decoder per single layer

class basicDecoder{

    protected:
    std::string archPath;
    
    public:
    basicDecoder(std::string archPath) : archPath(archPath){}
    virtual xxHashStruct fromPathHash(const std::string& path) = 0;
    virtual xxHashStruct fromHashPath(std::string hash) = 0;

    virtual std::string getImageHash() = 0;
};