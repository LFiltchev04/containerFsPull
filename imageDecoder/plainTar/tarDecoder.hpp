#pragma once

#include <string>
#include <vector>


#include <archive.h>
#include <archive_entry.h>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <sys/mman.h>

namespace fs = std::filesystem;

#include "../basicDecoder.hpp"

struct xxHashStruct {
    char xxHashID[8];
    uint16_t recordLen;
    std::string pathName;
};

class tarDecoder : public basicDecoder {

    void* mmapAddr;
    struct archive* arch;
    std::vector<xxHashStruct> fileOffsets;

    public:
    tarDecoder(const std::string& path);
    void trashTarDecoder(const std::string& path); //lonng sync process to make the go output sparse

    std::string fromPathHash(std::string path) override;
    std::string fromHashPath(std::string hash) override;
    void streamParser();

};