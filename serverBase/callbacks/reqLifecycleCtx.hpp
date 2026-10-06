#pragma once

#include <string>
#include <vector>
#include "../../utils/uringHandler.hpp"


struct reqLifecycleCtx{
    std::string path; // i dont want to manage that manually
    int openSqes;

    uint64_t fileSize = 0;
    uint64_t fileOffset = 0;

    resumeCtx* write; //holds a one-off write context to set up the network write call

    int *networkFd = nullptr;
    int *openFd = nullptr;
};