#include <string>
#include <vector>
#include <mutex>

#include "baseTreeNode.hpp"
#include <unordered_map>

class baseTree{
    std::string dumpfilePath;
    std::vector<baseTreeNode> entries; //owns actual memory, functions as a flat array
    std::unordered_map<std::string, baseTreeNode*> fastPositionLookup;
    std::mutex treeMutex; //only locked for writes of new entries, cache hit updates proceed on node lock level

    uint8_t dynHitCount;
    baseTreeNode *lastFive[5];
    public:
    baseTree(std::string dumpPath);
    
    void addEntry(std::string crrPath, std::string newPath);
    void predictNext(); //figure out returns qithout excessive mallocing into vectors
    void updateState(std::string transitionTarget, bool hit);

};