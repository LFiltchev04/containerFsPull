#include "tarDecoder.hpp"

tarReader::tarReader(const std::string& path) : basicDecoder(path){
    arch = archive_read_new();

}



void tarReader::trashfileDecoder(const std::string& path) {
    int resFd = open(path.c_str(), O_RDONLY);
    if (resFd < 0) {
        printf("couldnt open trashfile: %s\n", path.c_str());
        return;
    }

    xxHashStruct* xxHstruct = new xxHashStruct;
    int lineSz = sizeof(xxHashStruct);
    
    while (read(resFd, xxHstruct, lineSz) != EOF){
        //is truncated to make the network transfer a little easier
        fileOffsets[std::string(xxHstruct->xxHashID, 6)] = *xxHstruct;
    }


    close(resFd);
}

