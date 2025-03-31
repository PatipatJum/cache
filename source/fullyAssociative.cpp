#include "../headers/fullyAssociative.h"
#include "../headers/addressStructs.h"
#include "../headers/utilityFunctions.h"

#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;

FullyAssociativeCache::FullyAssociativeCache(int cacheSize, int blockSize, int addressSize)
{
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->addressSize = addressSize;

    offsetBits = (int)log2(blockSize);
    cacheLines = cacheSize / blockSize;
    tagBits = addressSize - offsetBits;

    cache.resize(cacheLines);
    hits = 0;
    misses = 0;
    replaces = 0;
    accessCount = 0;
};

void FullyAssociativeCache::accessMemory(int deciAddr)
{
    accessCount++;
    string binaAddr = decimalToBinary(deciAddr, addressSize);
    accessHistory.push_back({deciAddr, binaAddr});

    // In fully associative cache, there's only tag and offset (no index)
    string tag = binaAddr.substr(0, tagBits);
    string offset = binaAddr.substr(tagBits, offsetBits);

    // Create an AddressInfo struct to maintain consistency
    AddressInfo info = {tag, "", offset};

    string result;

    // Check if there's a hit in any cache line
    bool hit = false;
    int hitLine = -1;

    for (int i = 0; i < cacheLines; i++)
    {
        if (cache[i].getValid() && cache[i].getTag() == tag)
        {
            hit = true;
            hitLine = i;
            break;
        }
    }

    if (hit)
    {
        // Cache hit
        hits++;
        result = "Hit";
        // Update last used time for this entry
        cache[hitLine].setLastUsed(accessCount);
    }
    else
    {
        // Cache miss
        misses++;

        // Find where to place the new block
        int replaceLine = -1;

        // First check if there's an invalid entry
        for (int i = 0; i < cacheLines; i++)
        {
            if (!cache[i].getValid())
            {
                replaceLine = i;
                break;
            }
        }

        // If all entries are valid, use LRU replacement policy
        if (replaceLine == -1)
        {
            int lruTime = accessCount + 1;

            for (int i = 0; i < cacheLines; i++)
            {
                if (cache[i].getLastUsed() < lruTime)
                {
                    lruTime = cache[i].getLastUsed();
                    replaceLine = i;
                }
            }

            // Need to replace a valid block
            replaces++;
            result = "Miss: replace(tag: " + cache[replaceLine].getTag() + ")";
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
        cache[replaceLine].setValid(true);
        cache[replaceLine].setTag(tag);
        cache[replaceLine].setData(data);
        cache[replaceLine].setLastUsed(accessCount);
    }

    resultHistory.push_back(result);
}

void FullyAssociativeCache::printAccessHistory() const
{
    cout << "Access History:" << endl;
    cout << "===================================================================================================================" << endl;
    cout << "| Access # |" << setw(addressSize + 3) << "Address |" << setw(tagBits + 3) << "Tag |" << setw(offsetBits + 4) << " Offset | Result " << endl;
    cout << "===================================================================================================================" << endl;

    for (int i = 0; i < accessHistory.size(); i++)
    {
        string address = accessHistory[i].binary;
        string tag = address.substr(0, tagBits);
        string offset = address.substr(tagBits, offsetBits);

        cout << "| " << setw(8) << i + 1 << " | ";
        cout << setw(11) << address << " | ";
        cout << setw(5) << tag << " | ";
        cout << setw(6) << offset << " | ";
        cout << setw(20) << left << resultHistory[i] << endl;
        cout << right;
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

void FullyAssociativeCache::printCacheStatus()
{
    cout << "Cache Status:" << endl;
    cout << "===================================================================================================================" << endl;
    cout << "| Line | Valid | " << setw(tagBits) << "Tag" << " | " << setw(16) << "Data" << " | Last Used |" << endl;
    cout << "===================================================================================================================" << endl;

    for (int i = 0; i < cacheLines; i++)
    {
        cout << "| " << setw(4) << i << " | ";
        cout << setw(5) << (cache[i].getValid() ? "Yes" : "No") << " | ";

        string tag = cache[i].getTag().empty() ? "Empty" : cache[i].getTag();
        cout << setw(tagBits) << tag << " | ";

        string data = cache[i].getValid() ? cache[i].getData() : "Empty";
        cout << setw(16) << data << " | ";

        cout << setw(9) << (cache[i].getValid() ? to_string(cache[i].getLastUsed()) : "-") << " |" << endl;
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

void FullyAssociativeCache::showCacheConfiguration() const
{
    cout << "====================FullyAssociativeCache====================" << endl;
    cout << "Cache Configuration:" << endl;
    cout << "- Cache Size: " << cacheSize << " bytes" << endl;
    cout << "- Block Size: " << blockSize << " bytes" << endl;
    cout << "- Address Size: " << addressSize << " bits" << endl;
    cout << "- Number of Lines: " << cacheLines << endl;
    cout << "- Associativity: Fully Associative" << endl;
    cout << "- Tag Size: " << tagBits << " bits" << endl;
    cout << "- Offset Size: " << offsetBits << " bits" << endl;
    cout << "============================================================" << endl;
    cout << endl;
}

void FullyAssociativeCache::clearCache()
{
    for (auto &block : cache)
    {
        block.setValid(false);
        block.setTag("");
        block.setData("");
        block.setLastUsed(0);
    }
    hits = 0;
    misses = 0;
    replaces = 0;
    accessCount = 0;
    accessHistory.clear();
    resultHistory.clear();
}