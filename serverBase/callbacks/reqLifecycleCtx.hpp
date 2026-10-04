#include <string>
#include <vector>
#include "../../utils/uringHandler.hpp"


struct reqLifecycleCtx{
    std::string path; // i dont want to manage that manually
    int openSqes;

    bool sizeKnown = false;
    uint64_t fileSize = 0;
    uint64_t fileOffset = 0; // bytes already handed to nghttp2 as DATA frames

    std::vector<resumeCtx*> uringCtxs;
    size_t nextFrameCtx = 0; // index of the chunk the next DATA frame belongs to
};