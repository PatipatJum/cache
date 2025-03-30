#include <bits/stdc++.h>
using namespace std;

class CacheBlock
{
private:
    bool valid;
    string tag;
    string data;
    int lastUsed;

public:
    CacheBlock() : valid(false), tag(""), data(""), lastUsed(0) {}

    bool getValid() const
    {
        return valid;
    }
    string getTag() const
    {
        return tag;
    }
    string getData() const
    {
        return data;
    }
    int getLastUsed() const
    {
        return lastUsed;
    }
    void setValid(bool newValid)
    {
        valid = newValid;
    }
    void setTag(string newTag)
    {
        tag = newTag;
    }
    void setData(string newData)
    {
        data = newData;
    }
    void setLastUsed(int time)
    {
        lastUsed = time;
    }
};

struct Address
{
    int decimal;
    string binary;
};

struct AddressInfo
{
    string tag;
    string index;
    string offset;
};
// convert decimal Number to binary string
string decimalToBinary(int deciAddr, int addressSize)
{
    bitset<32> binary(deciAddr);
    string binaAddr = binary.to_string();
    if (addressSize <= 32)
    {
        binaAddr = binary.to_string().substr(32 - addressSize);
    }
    return binaAddr;
}
// find tag, index, offset
AddressInfo parseAddress(const string &binAddr, int tagBits, int indexBits, int offsetBits)
{
    return {
        binAddr.substr(0, tagBits),                     // Tag
        binAddr.substr(tagBits, indexBits),             // Index
        binAddr.substr(tagBits + indexBits, offsetBits) // Offset
    };
}

//--------------------------------------------------------------DirectMappingCache--------------------------------------------------------------
class DirectMappingCache
{
private:
    vector<CacheBlock> cache;

    int cacheSize, blockSize, addressSize; // bytes bytes bits

    int tagBits, indexBits, offsetBits; // bits

    int hits, misses, replaces, cacheLine;
    int accessCount;

    vector<Address> accessHistory;
    vector<string> resultHistory;

public:
    DirectMappingCache(int cacheSize, int blockSize, int addressSize)
    {
        this->cacheSize = cacheSize;
        this->blockSize = blockSize;
        this->addressSize = addressSize;

        offsetBits = (int)log2(blockSize);
        cacheLine = cacheSize / blockSize;
        indexBits = (int)log2(cacheLine);
        tagBits = addressSize - indexBits - offsetBits;

        cache.resize(cacheLine);

        hits = 0;
        misses = 0;
        replaces = 0;
        accessCount = 0;
    };

    void accessMemory(int deciAddr)
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

    void printAccessHistory() const
    {
        //sho Accecss History table
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

    void printCacheStatus()
    {
        //show Cache statues table
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

    void showCacheConfiguration() const
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

    int getHit()
    {
        return hits;
    }

    int getMiss()
    {
        return misses;
    }

    int getReplace()
    {
        return replaces;
    }
};

//--------------------------------------------------------------NWaySetAssociative--------------------------------------------------------------------
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
    vector<Address> accessHistory;
    vector<string> resultHistory;

public:
    NWaySetAssociative(int cacheSize, int blockSize, int addressSize, int nWay)
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

    void accessMemory(int deciAddr)
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

    void printAccessHistory() const
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

    void printCacheStatus()
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

    void showCacheConfiguration() const
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

    int getHit()
    {
        return hits;
    }

    int getMiss()
    {
        return misses;
    }

    int getReplace()
    {
        return replaces;
    }
};

//--------------------------------------------------------------FullyAssociativeCache--------------------------------------------------------------------
class FullyAssociativeCache
{
private:
    vector<CacheBlock> cache;
    int cacheSize, blockSize, addressSize;
    int tagBits, offsetBits;
    int cacheLines;
    int hits, misses, replaces;
    int accessCount;
    vector<Address> accessHistory;
    vector<string> resultHistory;

public:
    FullyAssociativeCache(int cacheSize, int blockSize, int addressSize)
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

    void accessMemory(int deciAddr)
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

    void printAccessHistory() const
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

    void printCacheStatus()
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

    void showCacheConfiguration() const
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

    int getHit()
    {
        return hits;
    }

    int getMiss()
    {
        return misses;
    }

    int getReplace()
    {
        return replaces;
    }
};

class CacheSimulator
{
public:
    CacheSimulator() : directCache(nullptr), nWayCache(nullptr), fullyAssociativeCache(nullptr) {}

    ~CacheSimulator()
    {
        delete directCache;
        delete nWayCache;
        delete fullyAssociativeCache;
    }

    void run()
    {
        setupCache();
        showMenu();
    }

private:
    int cacheSize, blockSize, addressSize;
    DirectMappingCache *directCache;
    NWaySetAssociative *nWayCache;
    FullyAssociativeCache *fullyAssociativeCache;
    int cacheType; // 1 = Direct, 2 = N-Way, 3 = Fully Associative
    int nWay;      // Associativity for N-Way

    // Setup cache configuration
    void setupCache()
    {
        cout << "Input cache size (bytes): ";
        cin >> cacheSize;
        cout << "Input block size (bytes): ";
        cin >> blockSize;
        cout << "Input address size (bits): ";
        cin >> addressSize;

        cout << "Select cache type:" << endl;
        cout << "1. Direct Mapped" << endl;
        cout << "2. N-Way Set Associative" << endl;
        cout << "3. Fully Associative" << endl;
        cout << "Enter choice: ";
        cin >> cacheType;

        if (cacheType == 1)
        {
            directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
            directCache->showCacheConfiguration();
        }
        else if (cacheType == 2)
        {
            cout << "Enter associativity (N): ";
            cin >> nWay;
            nWayCache = new NWaySetAssociative(cacheSize, blockSize, addressSize, nWay);
            nWayCache->showCacheConfiguration();
        }
        else if (cacheType == 3)
        {
            fullyAssociativeCache = new FullyAssociativeCache(cacheSize, blockSize, addressSize);
            fullyAssociativeCache->showCacheConfiguration();
        }
    }

    // Show menu
    void showMenu()
    {
        int choice;
        do
        {
            cout << "\nMenu:" << endl;
            cout << "1. Access addresses" << endl;
            cout << "2. Show cache status" << endl;
            cout << "3. Show access history" << endl;
            cout << "4. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            handleMenuChoice(choice);
        } while (choice != 4);
    }

    // Handle menu choices
    void handleMenuChoice(int choice)
    {
        switch (choice)
        {
        case 1:
            accessAddresses();
            break;
        case 2:
            showCacheStatus();
            break;
        case 3:
            showAccessHistory();
            break;
        case 4:
            cout << "Exiting program." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    // Redirect to appropriate cache's status method
    void showCacheStatus()
    {
        if (cacheType == 2)
        {
            nWayCache->printCacheStatus();
        }
        else if (cacheType == 3)
        {
            fullyAssociativeCache->printCacheStatus();
        }
        else
        {
            directCache->printCacheStatus();
        }
    }

    // Redirect to appropriate cache's history method
    void showAccessHistory()
    {
        if (cacheType == 2)
        {
            nWayCache->printAccessHistory();
        }
        else if (cacheType == 3)
        {
            fullyAssociativeCache->printAccessHistory();
        }
        else
        {
            directCache->printAccessHistory();
        }
    }

    // Access memory addresses
    void accessAddresses()
    {
        int choice;
        cout << "1. Enter addresses manually" << endl;
        cout << "2. Use predefined addresses" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int numAccesses;
            cout << "Enter number of memory accesses: ";
            cin >> numAccesses;

            for (int i = 0; i < numAccesses; i++)
            {
                int address;
                cout << "Enter memory address " << (i + 1) << ": ";
                cin >> address;

                if (cacheType == 2)
                {
                    nWayCache->accessMemory(address);
                }
                else if (cacheType == 3)
                {
                    fullyAssociativeCache->accessMemory(address);
                }
                else
                {
                    directCache->accessMemory(address);
                }
            }
        }
        else
        {
            // Predefined addresses for testing
            int addresses[] = {112, 128, 104, 400, 132, 376, 149, 3840, 100};
            // int addresses[] = {0, 4, 16, 132, 232, 160, 1024, 30, 140, 3100, 180, 2180};

            int numAddresses = sizeof(addresses) / sizeof(addresses[0]);

            for (int i = 0; i < numAddresses; i++)
            {
                cout << "Memory address " << (i + 1) << ": " << addresses[i] << endl;

                if (cacheType == 2)
                {
                    nWayCache->accessMemory(addresses[i]);
                }
                else if (cacheType == 3)
                {
                    fullyAssociativeCache->accessMemory(addresses[i]);
                }
                else
                {
                    directCache->accessMemory(addresses[i]);
                }
            }
        }
    }
};

int main()
{
    cout << "Cache Memory Simulator" << endl;
    cout << "======================" << endl;

    CacheSimulator simulator;
    simulator.run();

    return 0;
}