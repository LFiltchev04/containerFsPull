#pragma once

#include <string>
#include <vector>


#include <archive.h>
#include <archive_entry.h>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <sys/mman.h>
#define XXH_INLINE_ALL
#include "../../utils/xxhash.h"

namespace fs = std::filesystem;

#include "../basicDecoder.hpp"

class tarDecoder : public basicDecoder {

    void* mmapAddr = nullptr;
    struct archive* arch;
    std::vector<xxHashStruct> fileOffsets;
    std::string tarPath;

    public:
    tarDecoder(const std::string& path);
    void trashTarDecoder(const std::string& path); //lonng sync process to make the go output sparse

    xxHashStruct fromPathHash(const std::string& path) override;
    xxHashStruct fromHashPath(std::string hash) override;
    void streamParser();

};