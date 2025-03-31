#include "../headers/cacheBlock.h"
using namespace std;

CacheBlock::CacheBlock() : valid(false), tag(""), data(""), lastUsed(0) {}

bool CacheBlock::getValid() const
{
    return valid;
}
string CacheBlock::getTag() const
{
    return tag;
}
string CacheBlock::getData() const
{
    return data;
}
int CacheBlock::getLastUsed() const
{
    return lastUsed;
}

void CacheBlock::setValid(bool newValid)
{
    valid = newValid;
}
void CacheBlock::setTag(string newTag)
{
    tag = newTag;
}
void CacheBlock::setData(string newData)
{
    data = newData;
}
void CacheBlock::setLastUsed(int time)
{
    lastUsed = time;
}