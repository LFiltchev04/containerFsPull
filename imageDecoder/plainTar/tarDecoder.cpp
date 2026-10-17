#include "tarDecoder.hpp"

tarDecoder::tarDecoder(const std::string& path) : basicDecoder(path){

}


std::string GLOBAL_BACKEND_PV_PATH = "/volumes/csi/tst/125eecc6-0ff2-41ca-bef6-14c6503f8b70";
std::string GLOBAL_BACKEND_PV_BLOBS = "/mnt/cephfs/volumes/csi/tst/125eecc6-0ff2-41ca-bef6-14c6503f8b70/docker/registry/v2/blobs/sha256";

//assumes a path in the user PV
void tarDecoder::trashTarDecoder(const std::string& hash) {
    std::string substr = hash.substr(0, 2);
    std::string mtbHsh = hash;
    
    std::string contDir = GLOBAL_BACKEND_PV_BLOBS + "/" + mtbHsh + "/" + substr + "/" + hash;
    int resFd = open((contDir + "/trashfile").c_str(), O_RDONLY);
    if (resFd < 0) {
        printf("couldnt open trashfile: %s\n", hash.c_str());
        return;
    }

    xxHashStruct* xxHstruct = new xxHashStruct;
    int lineSz = sizeof(xxHashStruct);
    
    while (read(resFd, xxHstruct, lineSz) > 0){
        fileOffsets.push_back(*xxHstruct);
    }
    close(resFd);

    
    

    int idxFd = open((contDir + "/fastMap").c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);

    //6 bytes of address range, 256tb VMA, lets see if it dies
    uintmax_t address_range_size = 1ULL << 48;
    ftruncate(idxFd, address_range_size);
    if (idxFd < 0) {
        printf("couldnt open fastMap: %s\n", (contDir + "/fastMap").c_str());
        return;
    }

    
    void* mapped = mmap(mmapAddr, address_range_size, PROT_READ | PROT_WRITE, MAP_SHARED, idxFd, 0);
    if (mapped == MAP_FAILED) {
        printf("mmap failed for fastMap: %s\n", (contDir + "/fastMap").c_str());
        close(idxFd);
        return;
    }


    //since its a 6 byte offset the xxHash maps fully to the memory range, disk persistent hashmap without overhead
    //this thing adds the vector entries into the map
    
    int opCount = 0;
    for (const auto& xxHstruct : fileOffsets) {
        uintmax_t offset = *reinterpret_cast<const uintmax_t*>(xxHstruct.xxHashID);
        memcpy(static_cast<char*>(mapped) + offset, &xxHstruct, sizeof(xxHashStruct));
        opCount++;
        if(opCount % 100 == 0){
            //flush to disk, make sure its persistent
            msync(mapped, address_range_size, MS_SYNC);
        }
    }

}

