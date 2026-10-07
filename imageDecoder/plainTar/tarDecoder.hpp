#pragma once

#include <string>
#include "../basicDecoder.hpp"



class tarReader : public basicDecoder {

    public:
    tarReader(const std::string& path);

    int offsetUpdate(int fileOffset) override;

};