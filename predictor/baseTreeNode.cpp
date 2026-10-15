#include "baseTreeNode.hpp"

baseTreeNode::baseTreeNode(std::string* fileName){
    name = fileName;

}

void baseTreeNode::addTransition(baseTreeNode* transTgt){
    transTgt->setActive();

    for(int x = 0; x < MAX_NODE_TRANSITIONS; x++){
        //checks if the transition exists already
        if(transition[x].getDst() == transTgt){
            transition[x].recordTransition();
            return;
        }
    }

    uint16_t lowestConn = 0;
    for(int x = 0; x < MAX_NODE_TRANSITIONS; x++){
        if(transition[x].getAccessCount() < transition[lowestConn].getAccessCount()){
            lowestConn = x;
        }
    }

    transition[lowestConn].initialize(this, transTgt);
    transition[lowestConn].recordTransition();
    //means the transition did not exist in this range
}