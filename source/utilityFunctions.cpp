#include "../headers/utilityFunctions.h"
#include <bitset>
#include <sstream>

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