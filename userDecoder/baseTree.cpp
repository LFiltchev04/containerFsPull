#include "baseTree.hpp"


baseTree::baseTree(std::string dumpPath){
    //figure out easy serialization at some point
}


void baseTree::addEntry(std::string crrPath, std::string newPath){
    std::lock_guard<std::mutex> l(treeMutex);

    for(int i = 0; i < 5; i++){
        if(lastFive[i]==nullptr){
            entries.emplace_back(newPath);
            lastFive[i] = &entries.back();
            fastPositionLookup[newPath] = &entries.back();
            
            break;
        }
    }
}

void baseTree::updateState(std::string transitionTarget, bool hit){
    entries.emplace_back(transitionTarget);
    fastPositionLookup[transitionTarget] = &entries.back();
            
    for(int x = 0; x < 5; x++){
        if(lastFive[x] == nullptr){ 
            lastFive[x] = &entries.back();
            break;
        }
    }

    //if it exited the loop, fifo of the queue

    for(int x = 0; x < 4; x++){
        lastFive[x] = lastFive[x+1];
    }
    lastFive[0] = &entries.back();
    lastFive[0]->incrTransCount(&transitionTarget, &hit);

    if(hit){
        dynHitCount++;
    }else{
        dynHitCount--;
    }

}


void baseTree::setOrigin(std::string originPath){
    if(entries.size() == 0){
        entries.emplace_back(originPath);
        lastFive[0] = &entries.back();
        fastPositionLookup[originPath] = &entries.back();
    }else{
        lastFive[0] = fastPositionLookup[originPath];
    }
}


void baseTree::addEntry(std::string crrPath, std::string newPath){
    
    
    std::lock_guard<std::mutex> l(treeMutex);

}