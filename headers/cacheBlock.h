#ifndef CACHEBLOCK_H
#define CACHEBLOCK_H

#include <string>
using namespace std;

class CacheBlock {
private:
    bool valid;
    string tag;
    string data;
    int lastUsed;

public:
    CacheBlock();
    
    bool getValid() const;
    string getTag() const;
    string getData() const;
    int getLastUsed() const;
    
    void setValid(bool newValid);
    void setTag(string newTag);
    void setData(string newData);
    void setLastUsed(int time);
};

#endif 