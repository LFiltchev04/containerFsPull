#include <string>
#include <vector>

#include "basicDecoder.hpp"
#include "plainTar/tarDecoder.hpp"

enum decoderType {
    PLAIN_TAR
};


class globalDecoderObj {
    std::string sourceDir; 
    std::vector <basicDecoder*> decoders;

    public:
    globalDecoderObj();

    void addDecoder(std::string location); //adds a decoder from a base file
    
    std::string getLayerFromHash(std::string xxHash); //returns layer from hash
    std::string getLayerFromPath(std::string path); //returns layer from path
    
    std::string getPathHash(std::string xxHash); //returns path from hash
    std::string getxxHashID(std::string path); //returns xxHash from path
    

};