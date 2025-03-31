#ifndef ADDRESSSTRUCTS_H
#define ADDRESSSTRUCTS_H

#include <string>
using namespace std;

struct address
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

#endif