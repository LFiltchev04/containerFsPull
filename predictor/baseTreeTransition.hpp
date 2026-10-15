#pragma once

#include <string>

#include "baseTreeNode.hpp"

class baseTreeTransition{
    baseTreeNode *src;
    baseTreeNode *dst;

    uint32_t accessCount;
    uint32_t hitCount;

    public:
    baseTreeTransition();
    
    void recordTransition();
    void initialize(baseTreeNode *s, baseTreeNode *d);
    
    baseTreeNode* getSrc();
    baseTreeNode* getDst();

    uint32_t *getAccessCount();
};