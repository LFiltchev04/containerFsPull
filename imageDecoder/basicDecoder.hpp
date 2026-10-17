#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>


std::string globalBackendPV;
//implements basic functionality for distribution backend traversal opens one decoder per single layer

class basicDecoder{

    protected:
    std::string archPath;
    
    public:
    basicDecoder(std::string archPath) : archPath(archPath){}
    virtual std::string fromPathHash(std::string path) = 0;
    virtual std::string fromHashPath(std::string hash) = 0;

};