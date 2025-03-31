#ifndef CACHESIMULATOR_H
#define CACHESIMULATOR_H

#include "../headers/directMapping.h"
#include "../headers/nWaySetAssociative.h"
#include "../headers/fullyAssociative.h"
#include <vector>

class CacheSimulator
{

private:
    vector<int> addresses;
    int cacheSize, blockSize, addressSize;
    DirectMappingCache *directCache;
    NWaySetAssociative *nWayCache;
    FullyAssociativeCache *fullyCache;
    int cacheType; // 1 = Direct, 2 = N-Way, 3 = Fully Associative
    int nWay;      // Associativity for N-Way

    void displayMenu() const;

    void setupAddresses();

    void setupCache();

    void cleanup();

    void selectCacheType();

    void simulateCache();

    void printCacheStatus() const;

    void printAccessHistory() const;

public:
    CacheSimulator();
    ~CacheSimulator();
    void run();
};
#endif