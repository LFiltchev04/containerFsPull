#include <string>
#include <vector>

#include "basicDecoder.hpp"
#include "plainTar/tarDecoder.hpp"

enum decoderType {
    PLAIN_TAR
};

std::string GLOBAL_BACKEND_PV = "/volumes/csi/tst/125eecc6-0ff2-41ca-bef6-14c6503f8b70";

class globalDecoderObj {
    std::string sourceDir; 
    std::vector <basicDecoder*> decoders;
    std::unordered_map<std::string, basicDecoder*> seenHashes;

    public:
    globalDecoderObj();

    bool seen(std::string hash);
    void addDecoder(std::string hash); //adds a decoder from a base file
    
    std::string getLayerFromHash(std::string xxHash); //returns layer from hash
    std::string getLayerFromPath(std::string path); //returns layer from path
    
    std::string getPathHash(std::string xxHash); //returns path from hash
    std::string getxxHashID(std::string path); //returns xxHash from path
    
    basicDecoder* getDecoder(std::string hash);
};