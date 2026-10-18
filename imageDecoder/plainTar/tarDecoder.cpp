#include "tarDecoder.hpp"

tarDecoder::tarDecoder(const std::string& path) : basicDecoder(path){

}


//assumes a path in the user PV
void tarDecoder::trashTarDecoder(const std::string& hash) {

    
    int resFd = open((hash + "/trashfile").c_str(), O_RDONLY);
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

    
    

    int idxFd = open((hash + "/fastMap").c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);

    //6 bytes of address range, 256tb VMA, lets see if it dies
    uintmax_t address_range_size = 1ULL << 48;
    ftruncate(idxFd, address_range_size);
    if (idxFd < 0) {
        printf("couldnt open fastMap: %s\n", (hash + "/fastMap").c_str());
        return;
    }

    
    void* mapped = mmap(mmapAddr, address_range_size, PROT_READ | PROT_WRITE, MAP_SHARED, idxFd, 0);
    if (mapped == MAP_FAILED) {
        printf("mmap failed for fastMap: %s\n", (hash + "/fastMap").c_str());
        close(idxFd);
        return;
    }


    mmapAddr = mapped;

    //since its a 6 byte offset the xxHash maps fully to the memory range, disk persistent hashmap without overhead
    //this thing adds the vector entries into the map
    
    int opCount = 0;
    for (const auto& xxHstruct : fileOffsets) {
        uintmax_t offset = 0;
        memcpy(&offset, xxHstruct.xxHashID, 6);
        memcpy(static_cast<char*>(mapped) + offset, &xxHstruct, XX_SLOT_SIZE);
        opCount++;
        if(opCount % 100 == 0){
            //flush to disk, make sure its persistent
            msync(mapped, address_range_size, MS_SYNC);
        }
    }

}

xxHashStruct tarDecoder::fromPathHash(const std::string& path) {
    uint64_t fullHash = XXH3_64bits(path.data(), path.size());

    // 6 byte truncate
    uint64_t slotIndex = fullHash & ((1ULL << 48) - 1);

    xxHashStruct result{};
    if (!mmapAddr) return result;
    memcpy(&result, static_cast<char*>(mmapAddr) + slotIndex, XX_SLOT_SIZE);
    return result;
}


xxHashStruct tarDecoder::fromHashPath(uint64_t hash) {

    // truncates to 6 bytes
    uint64_t slotIndex = hash & ((1ULL << 48) - 1);

    xxHashStruct result{};
    if (!mmapAddr) return result;
    memcpy(&result, static_cast<char*>(mmapAddr) + slotIndex, XX_SLOT_SIZE);

    return result;
}

