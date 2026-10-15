#include <string>


class baseTreeNode{
    
    public:
    baseTreeNode(std::string nodeName);
    void incrTransCount(std::string *transTgt, bool* hit);
};