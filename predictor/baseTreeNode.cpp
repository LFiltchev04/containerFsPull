#include "baseTreeNode.hpp"

baseTreeNode::baseTreeNode(std::string* fileName){
    name = fileName;

}

void baseTreeNode::addTransition(std::string *transTgt){

    for(int x = 0; x < MAX_NODE_TRANSITIONS; x++){
        if(transition[x].getAccessCount() == 0){
            if(transTgt == transition[x].getDst()){
                transition[x].recordTransition();
            }
        }
    }

    //means the transition did not exist in this range

}