#ifndef NWAYSETASSOCIATIVE_H
#define NWAYSETASSOCIATIVE_H
#include "../headers/addressStructs.h"
#include "../headers/cacheBlock.h"

#include <vector>
class NWaySetAssociative
{
private:
    vector<vector<CacheBlock>> cache;
    int cacheSize, blockSize, addressSize; // bytes bytes bits
    int tagBits, setBits, offsetBits;      // bits
    int cacheSet;
    int hits, misses, replaces;
    int accessCount;
    int associativity; // N-way
    vector<address> accessHistory;
    vector<string> resultHistory;

public:
    NWaySetAssociative(int cacheSize, int blockSize, int addressSize, int nWay);

    void accessMemory(int deciAddr);

    void printAccessHistory() const;

    void printCacheStatus();

    void showCacheConfiguration() const;

    void clearCache();
};
#endif