#include <string>

#include "baseTreeTransition.hpp"

#define MAX_NODE_TRANSITIONS 12

class baseTreeNode{

    bool isActive = false; //simple flag set when a node has something pointed to it, if its false, drop it
    std::string *name;
    baseTreeTransition transition[MAX_NODE_TRANSITIONS];
    

    public:
    baseTreeNode(std::string *nodeName);
    void addTransition(baseTreeNode* transTgt);
    void setActive();
    bool getActive();
    std::string* getAddr();
};