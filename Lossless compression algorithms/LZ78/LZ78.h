#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
using namespace std;

class LZ78
{
public:
	vector<uint8_t> encode(vector<uint8_t>& string);
	vector<uint8_t> decode(vector<uint8_t>& string);
};