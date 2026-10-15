#include "baseTreeTransition.hpp"


baseTreeTransition::baseTreeTransition(){
    accessCount = 0;
    hitCount = 0;
}

void baseTreeTransition::initialize(std::string *s, std::string *d){
    src = s;
    dst = d;
}

void baseTreeTransition::recordTransition(){
    accessCount++;
}



uint32_t *baseTreeTransition::getAccessCount(){
    return &accessCount;
}

std::string* baseTreeTransition::getSrc(){
    return src;
}

std::string* baseTreeTransition::getDst(){
    return dst;
}