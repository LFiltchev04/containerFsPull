#include "baseTree.hpp"


baseTree::baseTree(std::string dumpPath){
    //figure out easy serialization at some point
}

void baseTree::addEdge(std::string crrPath, std::string newPath){
    std::lock_guard<std::mutex> l(treeMutex);

    auto crrNode = fastPositionLookup.find(crrPath);
    auto newNode = fastPositionLookup.find(newPath);

    if((crrNode != fastPositionLookup.end()) and
       (newNode != fastPositionLookup.end())){

        entries->emplace_back(newPath);
        fastPositionLookup[newPath] = &entries->back();
    }

    if(newNode == fastPositionLookup.end()){
        entries->emplace_back(newPath);
        fastPositionLookup[newPath] = &entries->back();
    }


    &entries->back();
}


void baseTree::setOrigin(std::string originPath){


}
