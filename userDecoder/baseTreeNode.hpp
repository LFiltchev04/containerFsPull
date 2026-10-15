#include <string>


class baseTreeNode{
    
    public:
    baseTreeNode(std::string nodeName);
    void setOrigin(std::string originPath);
    void incrTransCount(std::string *transTgt, bool* hit);
};