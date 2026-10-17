#include "basicDecoder.hpp"


 #pragma pack(push, 1)
    struct packedRecord{
        uint8_t xxHashID[6];
        uint64_t fArchOffset;
        mode_t fileType;
        std::string recordName;
        uint16_t trimSize;
    };
#pragma pack(pop)


basicDecoder::basicDecoder(std::string hash){

    //just opens the base archive
    baseDirPath = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash;
    
    
    std::string temp = globalBackendPV + "/docker/registry/v2/blobs/sha256/" + hash.substr(0,2) + "/" + hash + "/index.offst"; 
    creat(temp.data(), 0644);
    int idxOffset = open(temp.data(), O_RDWR);
    

    struct archive *archPtr = archive_read_new();
    archive_read_support_format_tar(archPtr);
    archive_read_open_filename(archPtr, baseDirPath.data(), 10240);
    archive_read_support_compression_none(archPtr);

    struct archive_entry* entry;
    std::string fPathEntry;
    uint64_t fArchOffset=0;
    mode_t fileType;
    
   
    while (archive_read_next_header(archPtr, &entry) == ARCHIVE_OK) {
        fPathEntry = archive_entry_pathname(entry);
        fArchOffset = archive_read_header_position(archPtr);
        fileType = archive_entry_filetype(entry);

        if(fileType == AE_IFREG){
            //raw data dumps go to the lookup map
            

            packedRecord rec{
                
                .fArchOffset = fArchOffset,
                .fileType = fileType,
                .recordName = fPathEntry,
                .trimSize = 0
            };

        }
    }


}