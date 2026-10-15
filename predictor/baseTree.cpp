#include "baseTree.hpp"


baseTree::baseTree(std::string dumpPath){
    //figure out easy serialization at some point
}

void baseTree::addEdge(std::string crrPath, std::string newPath){
    std::lock_guard<std::mutex> l(treeMutex);

    auto crrNode = fastPositionLookup.find(crrPath);
    auto newNode = fastPositionLookup.find(newPath);

    //since its tapped directly to network i need to sanitize
    if((crrNode != fastPositionLookup.end()) and
       (newNode != fastPositionLookup.end())){

        //for combinatorial jump index
        lastTwo[0] = &crrPath;
        lastTwo[1] = &newPath;

        //inserts only for empty slots
        if(occupied < 12){
            owned[occupied] = newNode->second;
            occupied++;
        }

        //finds ones that already exists and increments them
        for(uint8_t x = 0; x < occupied; x++){
            if(owned[x] == crrNode->second){
                
                owned[x]->addTransition(newNode->second);
                break;
            }
        }

        uint32_t smallestIdx = 0;
        for(uint8_t x = 0; x < occupied; x++){
            if(owned[x]->getActive() < owned[smallestIdx]->getActive()){
                smallestIdx = x;
            }
        }


        if(crrNode->first == crrPath){
            owned[smallestIdx]->addTransition(newNode->second);
        }

    }
}


void baseTree::setOrigin(std::string originPath){


}
