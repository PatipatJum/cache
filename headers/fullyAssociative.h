#ifndef FULLYASSOCIATIVE_H
#define FULLYASSOCIATIVE_H
#include "../headers/addressStructs.h"
#include "../headers/cacheBlock.h"

#include <vector>

class FullyAssociativeCache
{
private:
    vector<CacheBlock> cache;
    int cacheSize, blockSize, addressSize;
    int tagBits, offsetBits;
    int cacheLines;
    int hits, misses, replaces;
    int accessCount;
    vector<address> accessHistory;
    vector<string> resultHistory;

public:
    FullyAssociativeCache(int cacheSize, int blockSize, int addressSize);

    void accessMemory(int deciAddr);

    void printAccessHistory() const;

    void printCacheStatus();

    void showCacheConfiguration() const;

    void clearCache();
};

#endif