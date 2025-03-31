#ifndef DIRECMAPPING_H
#define DIRECMAPPING_H
#include "../headers/addressStructs.h"
#include "../headers/cacheBlock.h"

#include <vector>
#include <string>
using namespace std;

class DirectMappingCache
{
private:
    vector<CacheBlock> cache;

    int cacheSize, blockSize, addressSize; // bytes bytes bits

    int tagBits, indexBits, offsetBits; // bits

    int hits, misses, replaces, cacheLine;
    int accessCount;

    vector<address> accessHistory;
    vector<string> resultHistory;

public:
    DirectMappingCache(int cacheSize, int blockSize, int addressSize);

    void accessMemory(int deciAddr);

    void printAccessHistory() const;

    void printCacheStatus();

    void showCacheConfiguration() const;
    
    void clearCache();
};
#endif