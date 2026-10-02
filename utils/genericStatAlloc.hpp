#include <stack>
#include <mutex>

template <typename T>
class staticAllocatedPool{
    std::mutex poolLock;
    uint32_t maxSize;
    uint32_t currentSize;
    T pool[maxSize];

    std::stack<T*> poolRef;
    

    public:
    staticAllocatedPool(int maxSize);

    T* get();
    void yield(T* item);

};