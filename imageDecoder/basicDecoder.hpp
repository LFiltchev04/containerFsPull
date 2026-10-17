#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>


std::string globalBackendPV;
//implements basic functionality for distribution backend traversal
class basicDecoder{

    protected:
    std::string baseDirPath;
    int baseOffsetFd;
    
    public:
    basicDecoder(std::string hash);
    virtual int offsetUpdate(int fileOffset) = 0; //does a lookup in the tar offset table to get the right offset position


};