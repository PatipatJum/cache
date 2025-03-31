#include "../headers/directMapping.h"
#include "../headers/addressStructs.h"
#include "../headers/utilityFunctions.h"

#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;

DirectMappingCache::DirectMappingCache(int cacheSize, int blockSize, int addrSize)
{
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->addressSize = addrSize;

    offsetBits = (int)log2(blockSize);
    cacheLine = cacheSize / blockSize;
    indexBits = (int)log2(cacheLine);
    tagBits = addressSize - indexBits - offsetBits;

    cache.resize(cacheLine);

    hits = 0;
    misses = 0;
    replaces = 0;
    accessCount = 0;
}

void DirectMappingCache::accessMemory(int deciAddr)
{
    accessCount++;
    // decimal to binary
    string binaAddr = decimalToBinary(deciAddr, addressSize);
    accessHistory.push_back({deciAddr, binaAddr});
    // find tag, index, offset
    AddressInfo info = parseAddress(binaAddr, tagBits, indexBits, offsetBits);
    string result;
    int index = stoi(info.index, nullptr, 2);

    // valid
    if (cache[index].getValid())
    {
        // cache hit
        if (cache[index].getTag() == info.tag)
        {
            hits++;
            result = "Hit";
        }
        else
        {
            // cache replace (miss)
            misses++;
            replaces++;
            result = "Miss: replace(tag: " + cache[index].getTag() + ")";
        }
    }
    else
    {
        // cache miss
        result = "Miss";
        misses++;
    }
    // set data
    resultHistory.push_back(result);
    int start = (deciAddr / blockSize) * blockSize;
    int end = start + blockSize - 1;
    string data = "M[" + to_string(start) + " - " + to_string(end) + "]";
    // updata cache
    cache[index].setValid(true);
    cache[index].setTag(info.tag);
    cache[index].setData(data);
    cache[index].setLastUsed(accessCount);
}

void DirectMappingCache::printAccessHistory() const
{
    // show Accecss History table
    cout << "Access History:" << endl;
    cout << "==========================================================" << endl;
    cout << "| Access # |" << setw(addressSize + 3) << "Address |" << setw(tagBits + 4) << "Tag | " << setw(indexBits) << "Index | " << setw(offsetBits + 4) << "Offset | Result " << endl;
    cout << "==========================================================" << endl;

    for (int i = 0; i < accessHistory.size(); i++)
    {
        string address = accessHistory[i].binary;
        AddressInfo info = parseAddress(address, tagBits, indexBits, offsetBits);

        cout << "| " << setw(8) << i + 1 << " | ";
        cout << setw(11) << address << " | ";
        cout << setw(5) << info.tag << " | ";
        cout << setw(5) << info.index << " | ";
        cout << setw(6) << info.offset << " | ";
        cout << setw(20) << left << resultHistory[i] << endl;
        cout << right;
    }

    cout << "==========================================================" << endl;

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

void DirectMappingCache::printCacheStatus()
{
    // show Cache statues table
    cout << "Cache Status:" << endl;
    cout << "========================================================================================" << endl;
    cout << "| Line | Valid | " << setw(tagBits) << "Tag" << " | " << setw(16) << "Data" << " |" << endl;
    cout << "========================================================================================" << endl;

    for (int i = 0; i < cacheLine; i++)
    {
        cout << "| " << setw(4) << i << " | ";
        cout << setw(5) << (cache[i].getValid() ? "Yes" : "No") << " | ";

        string tag = cache[i].getTag().empty() ? "Empty" : cache[i].getTag();
        cout << setw(tagBits) << tag << " | ";

        string data = cache[i].getValid() ? cache[i].getData() : "Empty";
        cout << setw(16) << data << " |" << endl;
    }
    cout << "========================================================================================" << endl;

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

void DirectMappingCache::showCacheConfiguration() const
{
    cout << "====================DirectMappingCache====================" << endl;
    cout << "Cache Configuration:" << endl;
    cout << "- Cache Size: " << cacheSize << " bytes" << endl;
    cout << "- Block Size: " << blockSize << " bytes" << endl;
    cout << "- Address Size: " << addressSize << " bits" << endl;
    cout << "- Number of Lines: " << cacheLine << endl;
    cout << "- Tag Size: " << tagBits << " bits" << endl;
    cout << "- Index Size: " << indexBits << " bits" << endl;
    cout << "- Offset Size: " << offsetBits << " bits" << endl;
    cout << "==========================================================" << endl;
    cout << endl;
}

void DirectMappingCache::clearCache()
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