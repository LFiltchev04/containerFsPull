#include <archive.h>
#include <archive_entry.h>

#include "tarDecoder.hpp"

tarReader::tarReader(const std::string& path) : basicDecoder(path){
    arch = archive_read_new();

}

int tarReader::offsetUpdate(int offset) {
    archive_read_support_format_tar(arch);
}