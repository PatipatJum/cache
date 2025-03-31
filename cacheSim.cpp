#include <bits/stdc++.h>
using namespace std;
// stored vaild tag data lastUsed
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
// the data structure use store the decimal
struct Address
{
    int decimal;
    string binary;
};
// the data structure use store the address compnentds in the cache
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

    void printCacheStatus()
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
    void clearCache()
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

    void clearCache()
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

    void clearCache()
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
};

class CacheSimulator
{
public:
    CacheSimulator() : directCache(nullptr), nWayCache(nullptr), fullyCache(nullptr), cacheSize(0), blockSize(0), addressSize(0) {}

    ~CacheSimulator()
    {
        cleanup();
    }

    void run()
    {
        setupCache();
        setupAddresses();

        bool running = true;
        while (running)
        {
            selectCacheType();
            simulateCache();
            
            int choice;
            do
            {
                displayMenu();
                cin >> choice;

                switch (choice)
                {
                case 1: // Show cache status
                    printCacheStatus();
                    break;
                case 2: // Show access history
                    printAccessHistory();
                    break;
                case 3: // Try different cache type
                    break;
                case 4: // New addresses
                    setupCache();
                    setupAddresses();
                    break;
                case 5: // Exit
                    running = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                }

            } while (choice != 3 && choice != 4 && choice != 5);
        }
    }

private:
    vector<int> addresses;
    int cacheSize, blockSize, addressSize;
    DirectMappingCache *directCache;
    NWaySetAssociative *nWayCache;
    FullyAssociativeCache *fullyCache;
    int cacheType; // 1 = Direct, 2 = N-Way, 3 = Fully Associative
    int nWay;      // Associativity for N-Way

    void displayMenu() const
    {
        cout << "\n========== Cache Simulator Options ==========\n";
        cout << "1. Show cache status\n";
        cout << "2. Show access history\n";
        cout << "3. Try different cache type with same addresses\n";
        cout << "4. Start over with new cache and addresses\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
    }

    void setupAddresses()
    {
        cout << "\n========== Address Configuration ==========\n";
        cout << "1. Enter addresses manually\n";
        cout << "2. Use predefined addresses\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;

        addresses.clear();
        
        if (choice == 1)
        {
            int numAddresses;
            cout << "Enter number of memory accesses: ";
            cin >> numAddresses;

            for (int i = 0; i < numAddresses; i++)
            {
                int address;
                cout << "Enter memory address " << (i + 1) << ": ";
                cin >> address;
                addresses.push_back(address);
            }
        }
        else
        {
            // Predefined address pattern
            addresses = {0, 4, 16, 132, 232, 160, 1024, 30, 140, 3100, 180, 2180};
            cout << "Using predefined addresses: ";
            for (int addr : addresses)
            {
                cout << addr << " ";
            }
            cout << endl;
        }
    }

    void setupCache()
    {
        cout << "\n========== Cache Configuration ==========\n";
        cout << "Input cache size (bytes): ";
        cin >> cacheSize;
        cout << "Input block size (bytes): ";
        cin >> blockSize;
        cout << "Input address size (bits): ";
        cin >> addressSize;

        cleanup(); // Clean up previous cache objects
    }

    void cleanup()
    {
        // Delete any previously allocated cache objects
        delete directCache;
        delete nWayCache;
        delete fullyCache;
        
        directCache = nullptr;
        nWayCache = nullptr;
        fullyCache = nullptr;
    }

    void selectCacheType()
    {
        cout << "\n========== Cache Type Selection ==========\n";
        cout << "1. Direct Mapped\n";
        cout << "2. N-Way Set Associative\n";
        cout << "3. Fully Associative\n";
        cout << "Enter choice: ";
        cin >> cacheType;

        // Clear any existing cache data
        if (directCache != nullptr) directCache->clearCache();
        if (nWayCache != nullptr) nWayCache->clearCache();
        if (fullyCache != nullptr) fullyCache->clearCache();

        // Create appropriate cache based on selection
        switch (cacheType)
        {
        case 1: // Direct Mapped
            if (!directCache) directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
            directCache->showCacheConfiguration();
            break;
        
        case 2: // N-Way Set Associative
            cout << "Enter associativity (N): ";
            cin >> nWay;
            delete nWayCache; // Delete previous instance if any
            nWayCache = new NWaySetAssociative(cacheSize, blockSize, addressSize, nWay);
            nWayCache->showCacheConfiguration();
            break;
        
        case 3: // Fully Associative
            if (!fullyCache) fullyCache = new FullyAssociativeCache(cacheSize, blockSize, addressSize);
            fullyCache->showCacheConfiguration();
            break;
        
        default:
            cout << "Invalid choice. Using Direct Mapped as default.\n";
            cacheType = 1;
            if (!directCache) directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
            directCache->showCacheConfiguration();
        }
    }

    void simulateCache()
    {
        cout << "\nSimulating memory access...\n";
        
        for (int addr : addresses)
        {
            switch (cacheType)
            {
            case 1:
                directCache->accessMemory(addr);
                break;
            case 2:
                nWayCache->accessMemory(addr);
                break;
            case 3:
                fullyCache->accessMemory(addr);
                break;
            }
        }
        
        cout << "Simulation complete.\n";
    }

    void printCacheStatus() const
    {
        switch (cacheType)
        {
        case 1:
            directCache->printCacheStatus();
            break;
        case 2:
            nWayCache->printCacheStatus();
            break;
        case 3:
            fullyCache->printCacheStatus();
            break;
        }
    }

    void printAccessHistory() const
    {
        switch (cacheType)
        {
        case 1:
            directCache->printAccessHistory();
            break;
        case 2:
            nWayCache->printAccessHistory();
            break;
        case 3:
            fullyCache->printAccessHistory();
            break;
        }
    }
};

int main()
{
    cout << "Cache Memory Simulator" << endl;
    CacheSimulator simulator;
    simulator.run();
    return 0;
}