#include "baseTreeTransition.hpp"


baseTreeTransition::baseTreeTransition(){
    accessCount = 0;
    hitCount = 0;
}

void baseTreeTransition::initialize(baseTreeNode *s, baseTreeNode *d){
    src = s;
    dst = d;
}

void baseTreeTransition::recordTransition(){
    accessCount++;
}



uint32_t *baseTreeTransition::getAccessCount(){
    return &accessCount;
}

baseTreeNode* baseTreeTransition::getSrc(){
    return src;
}

baseTreeNode* baseTreeTransition::getDst(){
    return dst;
}