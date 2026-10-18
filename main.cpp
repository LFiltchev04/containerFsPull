#include "serverBase/mainCall.hpp"


staticAllocatedPool<resumeCtx>* resumeCtxPool = nullptr;
staticAllocatedPool<batchWriteCtx>* batchWriteCtxPool = nullptr;

int main(int argc, char* argv[]){
    
    
    runApp();
    return 0;
}