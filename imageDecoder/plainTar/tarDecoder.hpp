#pragma once

#include <string>
#include "../basicDecoder.hpp"



class tarReader : public basicDecoder {

    struct archive* arch;
    public:
    tarReader(const std::string& path);

    int offsetUpdate(int fileOffset) override;

};