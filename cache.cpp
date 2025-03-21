#include <bits/stdc++.h>
using namespace std;

struct Address
{
    int decimal;
    string binary;
};

class CacheBlock
{
private:
    bool valid;
    string tag;
    string data;

public:
    CacheBlock() : valid(false), tag(""), data("") {}

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
};

class DirectMappingCache
{
private:
    vector<CacheBlock> cache;

    int cacheSize;
    int blockSize;
    int addressSize;

    int tagBits;
    int indexBits;
    int offsetBits;

    int hits;
    int misses;
    int replaces;
    int cacheLine;

    vector<Address> accessHistory;
    vector<string> resultHistory;

    struct AddressInfo
    {
        string tag;
        string index;
        string offset;
    };

    AddressInfo parseAddress(const string &binAddr) const
    {
        return {
            binAddr.substr(0, tagBits),                     // Tag
            binAddr.substr(tagBits, indexBits),             // Index
            binAddr.substr(tagBits + indexBits, offsetBits) // Offset
        };
    }

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
    };

    void accessMemory(int deciAddress)
    {
        bitset<32> binary(deciAddress);
        string binAddress = binary.to_string().substr(32 - addressSize);
        accessHistory.push_back({deciAddress, binAddress});

        AddressInfo info = parseAddress(binAddress);
        cout << info.tag << " " << info.index << " " << info.offset << endl;
        string result;
        int index = stoi(info.index, nullptr, 2);
        
        //vaild
        if (cache[index].getValid())
        {
            //cache hit
            if (cache[index].getTag() == info.tag)
            {
                hits++;
                result = "Hit";
            }
            else
            {
                //cache replace (miss)
                misses++;
                result = "Miss: replace( tag: " + to_string(stoi(info.tag, nullptr, 2)) + " )";
                replaces++;
            }
        }
        else
        {
            //cache miss
            result = "Miss";
            misses++;
        }
        resultHistory.push_back(result);

        int start = (deciAddress / blockSize) * blockSize;
        int end = start + blockSize - 1;
        string data = "M[" + to_string(start) + " - " + to_string(end) + "]";
        cache[index].setValid(true);
        cache[index].setTag(info.tag);
        cache[index].setData(data);
    }

    void printAccessHistory() const
    {
        cout << "Access History:" << endl;
        cout << "==========================================================" << endl;
        cout << "| Access # |   Address   |  Tag  | Index | Offset | Result |" << endl;
        cout << "==========================================================" << endl;

        for (int i = 0; i < accessHistory.size(); i++)
        {
            string address = accessHistory[i].binary;
            AddressInfo info = parseAddress(address);

            cout << "| " << setw(8) << i + 1 << " | ";
            cout << setw(11) << address << " | ";
            cout << setw(5) << info.tag << " | ";
            cout << setw(5) << stoi(info.index, nullptr, 2) << " | ";
            cout << setw(6) << info.offset << " | ";
            cout << setw(20) << left << resultHistory[i] << " |" << endl;
            cout << right;
        }

        cout << "==========================================================" << endl;

        // show hit, miss rate
        if (hits + misses > 0)
        {
            cout << "Hit rate: " << fixed << setprecision(2)
                 << (hits * 100.0 / (hits + misses)) << "% ("
                 << hits << " hits, " << misses << " misses)" << endl;
            cout << "Hit rate: " << fixed << setprecision(2)
                 << (misses * 100.0 / (hits + misses)) << "%" << endl;
        }
        else
        {
            cout << "No memory accesses yet." << endl;
        }
    }

    void printCacheStatus()
    {
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
            cout << "Hit rate: " << fixed << setprecision(2)
                 << (misses * 100.0 / (hits + misses)) << "%" << endl;
        }
        else
        {
            cout << "No memory accesses yet." << endl;
        }
    }

    void showCacheConfiguration() const
    {
        cout << "============================================" << endl;
        cout << "Cache Configuration:" << endl;
        cout << "- Cache Size: " << cacheSize << " bytes" << endl;
        cout << "- Block Size: " << blockSize << " bytes" << endl;
        cout << "- Address Size: " << addressSize << " bits" << endl;
        cout << "- Number of Lines: " << cacheLine << endl;
        cout << "- Tag Size: " << tagBits << " bits" << endl;
        cout << "- Index Size: " << indexBits << " bits" << endl;
        cout << "- Offset Size: " << offsetBits << " bits" << endl;
        cout << "============================================" << endl;
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
};

class TwoWaySetAssociative {};

class FullyAssociative{};

class CacheSimulator
{
public:
    CacheSimulator() : directCache(nullptr) {}
    ~CacheSimulator() { delete directCache; }
    void run()
    {
        setupCache();
        showMenu();
    }

private:
    int cacheSize, blockSize, addressSize;
    DirectMappingCache *directCache;

    // input cacheSize, blockSize, addressSize
    void setupCache()
    {
        cout << "input CacheSize (bytes): ";
        cin >> cacheSize;
        cout << "input blockSize (bytes): ";
        cin >> blockSize;
        cout << "input addressSize (bits): ";
        cin >> addressSize;

        directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
        directCache->showCacheConfiguration();
    }
    // show Menu
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
    // all handle function
    void handleMenuChoice(int choice)
    {
        switch (choice)
        {
        case 1:
            accessAddresses();
            break;
        case 2:
            directCache->printCacheStatus();
            break;
        case 3:
            directCache->printAccessHistory();
            break;

        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
    // input Addresses
    void accessAddresses()
    {
        int numAccesses;
        cout << "Enter number of memory accesses: ";
        cin >> numAccesses;

        for (int i = 0; i < numAccesses; i++)
        {
            int address;
            cout << "Enter memory address " << (i + 1) << ": ";
            cin >> address;
            directCache->accessMemory(address);
        }
    };
};

int main()
{
    CacheSimulator simulator;
    simulator.run();
}