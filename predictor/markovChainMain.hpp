#include <vector>
#include <string>
#include <unordered_map>
#include "baseTree.hpp"


struct treeEntry{
    baseTree* tree;
    bool changed = false;
    int accessEvents = 0;
};

class markovChain{
    int numFiles;
    std::unordered_map<std::string, baseTree> treeRanges;
    std::vector<std::string> fileMasterRecord; 
    
    std::vector<baseTree> readOnlySet;
    std::vector<treeEntry> writeOnlyTree;

    public:

    markovChain(); //deserializes
    markovChain(int numFiles): numFiles(numFiles) {}; //initializes with a given number of files only before first serialization

};