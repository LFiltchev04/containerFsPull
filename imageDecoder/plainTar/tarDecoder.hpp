#pragma once

#include <string>



class tarReader {
    std::string tarPath;
    std::string metaPath;

    uint64_t offsetPosition;

    public:
    tarReader(const std::string& path) : tarPath(path), offsetPosition(0){}

    void seekGuide(int *fd, std::string& targetPath);

};