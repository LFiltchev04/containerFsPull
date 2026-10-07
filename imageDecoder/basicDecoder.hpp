#include <string>
#include <unordered_map>



std::string globalBackendPV;
//implements basic functionality for distribution backend traversal
class basicDecoder{

    public:
    basicDecoder(std::string hash);
    virtual int offsetUpdate(int fileOffset) = 0; //does a lookup in the tar offset table to get the right offset position


};