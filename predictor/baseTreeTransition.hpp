#include <string>


class baseTreeTransition{
    std::string *src;
    std::string *dst;

    uint32_t accessCount;
    uint32_t hitCount;

    public:
    baseTreeTransition();
    
    void recordTransition();
    void initialize(std::string *s, std::string *d);
    
    std::string* getSrc();
    std::string* getDst();

    uint32_t *getAccessCount();
};