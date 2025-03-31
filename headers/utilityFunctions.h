#ifndef ADDRESSSTUCTS_H
#define ADDRESSSTUCTS_H

#include <vector>
#include <string>
#include "../headers/addressStructs.h"
using namespace std;

string decimalToBinary(int deciAddr, int addressSize);
AddressInfo parseAddress(const string &binAddr, int tagBits, int indexBits, int offsetBits);
#endif
