#include "../headers/nWaySetAssociative.h"
#include "../headers/addressStructs.h"
#include "../headers/utilityFunctions.h"

#include "iostream"
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;
NWaySetAssociative::NWaySetAssociative(int cacheSize, int blockSize, int addressSize, int nWay)
{
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->addressSize = addressSize;
    this->associativity = nWay;

    offsetBits = (int)log2(blockSize);
    cacheSet = (cacheSize / blockSize) / associativity;
    setBits = (int)log2(cacheSet);
    tagBits = addressSize - setBits - offsetBits;

    cache.resize(cacheSet, vector<CacheBlock>(associativity));
    hits = 0;
    misses = 0;
    replaces = 0;
    accessCount = 0;
};

void NWaySetAssociative::accessMemory(int deciAddr)
{
    accessCount++;
    string binaAddr = decimalToBinary(deciAddr, addressSize);
    accessHistory.push_back({deciAddr, binaAddr});

    AddressInfo info = parseAddress(binaAddr, tagBits, setBits, offsetBits);
    string result;
    int index = stoi(info.index, nullptr, 2);

    // Check if there's a hit in any of the ways
    bool hit = false;
    int hitWay = -1;

    for (int way = 0; way < associativity; way++)
    {
        // current tag matches the tag in the cache
        if (cache[index][way].getValid() && cache[index][way].getTag() == info.tag)
        {
            hit = true;
            hitWay = way;
            break;
        }
    }

    if (hit)
    {
        // Cache hit
        hits++;
        result = "Hit";
        // Update last used time for this entry
        cache[index][hitWay].setLastUsed(accessCount);
    }
    else
    {
        // Cache miss
        misses++;

        // Find where to place the new block
        int replaceWay = -1;

        // First check if there's an invalid entry
        for (int way = 0; way < associativity; way++)
        {
            if (!cache[index][way].getValid())
            {
                replaceWay = way;
                break;
            }
        }

        // If all entries are valid, use LRU replace
        if (replaceWay == -1)
        {
            int lruTime = accessCount + 1;

            for (int way = 0; way < associativity; way++)
            {
                if (cache[index][way].getLastUsed() < lruTime)
                {
                    lruTime = cache[index][way].getLastUsed();
                    replaceWay = way;
                }
            }

            // Need to replace a valid block
            replaces++;
            result = "Miss: replace(tag: " + cache[index][replaceWay].getTag() + ")";
        }
        else
        {
            result = "Miss";
        }

        // Calculate block address range for the data
        int start = (deciAddr / blockSize) * blockSize;
        int end = start + blockSize - 1;
        string data = "M[" + to_string(start) + " - " + to_string(end) + "]";

        // Update cache
        cache[index][replaceWay].setValid(true);
        cache[index][replaceWay].setTag(info.tag);
        cache[index][replaceWay].setData(data);
        cache[index][replaceWay].setLastUsed(accessCount);
    }

    resultHistory.push_back(result);
}

void NWaySetAssociative::printAccessHistory() const
{
    cout << "Access History:" << endl;
    cout << "====================================================================================================================" << endl;
    cout << "| Access # |" << setw(addressSize + 3) << "Address |" << setw(tagBits + 4) << "Tag | " << setw(5) << "  Set | " << setw(offsetBits + 4) << "Offset | Result " << endl;
    cout << "====================================================================================================================" << endl;

    for (int i = 0; i < accessHistory.size(); i++)
    {
        string address = accessHistory[i].binary;
        AddressInfo info = parseAddress(address, tagBits, setBits, offsetBits);

        cout << "| " << setw(8) << i + 1 << " | ";
        cout << setw(11) << address << " | ";
        cout << setw(5) << info.tag << " | ";
        cout << setw(5) << info.index << " | ";
        cout << setw(6) << info.offset << " | ";
        cout << setw(20) << left << resultHistory[i] << " |" << endl;
        cout << right;
    }

    cout << "====================================================================================================================" << endl;

    // show hit, miss rate
    if (hits + misses > 0)
    {
        cout << "Hit rate: " << fixed << setprecision(2)
             << (hits * 100.0 / (hits + misses)) << "% ("
             << hits << " hits, " << misses << " misses)" << endl;
        cout << "Miss rate: " << fixed << setprecision(2)
             << (misses * 100.0 / (hits + misses)) << "%" << endl;
        cout << "Replace count: " << replaces << endl;
    }
    else
    {
        cout << "No memory accesses yet." << endl;
    }
}

void NWaySetAssociative::showCacheConfiguration() const
{
    cout << "====================" << associativity << "-WaySetAssociative====================" << endl;
    cout << "Cache Configuration:" << endl;
    cout << "- Cache Size: " << cacheSize << " bytes" << endl;
    cout << "- Block Size: " << blockSize << " bytes" << endl;
    cout << "- Address Size: " << addressSize << " bits" << endl;
    cout << "- Number of Sets: " << cacheSet << endl;
    cout << "- Associativity: " << associativity << "-way" << endl;
    cout << "- Tag Size: " << tagBits << " bits" << endl;
    cout << "- Set Bits: " << setBits << " bits" << endl;
    cout << "- Offset Size: " << offsetBits << " bits" << endl;
    cout << "============================================================" << endl;
    cout << endl;
}

void NWaySetAssociative::printCacheStatus()
{
    cout << "Cache Status:" << endl;
    cout << "===================================================================================================================" << endl;
    cout << "| Set | Way | Valid | " << setw(tagBits) << "Tag" << " | " << setw(16) << "Data" << " | Last Used |" << endl;
    cout << "===================================================================================================================" << endl;

    for (int setIndex = 0; setIndex < cacheSet; setIndex++)
    {
        for (int way = 0; way < associativity; way++)
        {
            cout << "| " << setw(3) << setIndex << " | ";
            cout << setw(3) << way << " | ";
            cout << setw(5) << (cache[setIndex][way].getValid() ? "Yes" : "No") << " | ";

            string tag = cache[setIndex][way].getTag().empty() ? "Empty" : cache[setIndex][way].getTag();
            cout << setw(tagBits) << tag << " | ";

            string data = cache[setIndex][way].getValid() ? cache[setIndex][way].getData() : "Empty";
            cout << setw(16) << data << " | ";

            cout << setw(9) << (cache[setIndex][way].getValid() ? to_string(cache[setIndex][way].getLastUsed()) : "-") << " |" << endl;
        }
    }
    cout << "===================================================================================================================" << endl;

    // show hit, miss rate
    if (hits + misses > 0)
    {
        cout << "Hit rate: " << fixed << setprecision(2)
             << (hits * 100.0 / (hits + misses)) << "% ("
             << hits << " hits, " << misses << " misses)" << endl;
        cout << "Miss rate: " << fixed << setprecision(2)
             << (misses * 100.0 / (hits + misses)) << "%" << endl;
        cout << "Replace count: " << replaces << endl;
    }
    else
    {
        cout << "No memory accesses yet." << endl;
    }
}

void NWaySetAssociative::clearCache()
{
    for (auto &set : cache)
    {
        for (auto &block : set)
        {
            block.setValid(false);
            block.setTag("");
            block.setData("");
            block.setLastUsed(0);
        }
    }
    hits = 0;
    misses = 0;
    replaces = 0;
    accessCount = 0;
    accessHistory.clear();
    resultHistory.clear();
}