#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

#include "baseTreeNode.hpp"
#include "baseTreeTransition.hpp"

class baseTree{
    std::string dumpfilePath;
    
    std::vector<baseTreeNode>* entries; //owns actual memory, functions as a flat array

    std::unordered_map<std::string, baseTreeNode*> fastPositionLookup;
    std::mutex treeMutex; //only locked for writes of new entries, cache hit updates proceed on node lock level

    std::string* lastTwo[2];    

    public:
    baseTree(std::string dumpPath);
    
    void setOrigin(std::string originPath); //so you can start somewhere, only for initialization
    
    void addEntry(std::string crrPath);
    void addEdge(std::string crrPath, std::string newPath);

    void predictNext(); //figure out returns qithout excessive mallocing into vectors
    void updateState(std::string transitionTarget, bool hit);



};