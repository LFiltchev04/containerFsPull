#pragma once

#include <string>
#include <unordered_map>


#include <archive.h>
#include <archive_entry.h>
#include <cstdint>
#include <cstring>


#include "../basicDecoder.hpp"

struct xxHashStruct {
    char xxHashID[8];
    uint16_t recordLen;
    std::string pathName;
};

class tarReader : public basicDecoder {

    struct archive* arch;
    std::unordered_map<std::string, xxHashStruct> fileOffsets;

    public:
    tarReader(const std::string& path);
    void trashfileDecoder(const std::string& path); //lonng sync process to make the go output sparse

    int offsetUpdate(int fileOffset) override;

};