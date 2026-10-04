#pragma once

#include <stack>
#include <mutex>
#include <cstdint>
#include <cstdio>
#include <memory>

template <typename T>
class staticAllocatedPool{
    std::mutex poolLock;
    uint32_t maxSize;
    std::unique_ptr<T[]> pool;
    std::stack<T*> poolRef;

    public:
    staticAllocatedPool(int maxSize) : maxSize(maxSize), pool(new T[maxSize]) {
        for(int i = 0; i < maxSize; i++){
            poolRef.push(&pool[i]);
        }
    }

    T* get(){
        std::lock_guard<std::mutex> lock(poolLock);
        if(poolRef.empty()){
            return nullptr;
        }
        T* destination = poolRef.top();
        poolRef.pop();
        return destination;
    }

    void yield(T* item){
        if(item < pool.get() || item >= pool.get() + maxSize){
            printf("Invalid item passed to yield, ignoring\n");
            return;
        }
        std::lock_guard<std::mutex> lock(poolLock);
        poolRef.push(item);
    }
};
