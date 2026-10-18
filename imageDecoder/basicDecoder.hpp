#pragma once
#include <string>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <fcntl.h>


//the standard struct for a file entry, if no archive is used the fileSize or fileOffset are to be set to 0
struct xxHashStruct {
    char xxHashID[8]; //the hashID of the path
    uint64_t fileSize; //the size of the file
    uint64_t fileOffset; //the offset of the file inside of a single archive file if applicable
    uint16_t recordLen; //the length of the path string of the file
    std::string pathName; // the raw bytes of the path string
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
    virtual xxHashStruct fromPathHash(const std::string& path) = 0; //hashesh a path, returns file metadata
    virtual xxHashStruct fromHashPath(uint64_t hash) = 0; // resolves a hash returns metadata



    virtual std::string getImageHash() = 0;
};