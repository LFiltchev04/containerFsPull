#include "baseTreeNode.hpp"

baseTreeNode::baseTreeNode(std::string* fileName){
    name = fileName;

}

void baseTreeNode::addTransition(baseTreeNode* transTgt){
    transTgt->setActive();

    for(int x = 0; x < MAX_NODE_TRANSITIONS; x++){
        //checks if the transition exists already
        if(transition[x].getDst() == transTgt){
            
        }

    }

    //means the transition did not exist in this range

}