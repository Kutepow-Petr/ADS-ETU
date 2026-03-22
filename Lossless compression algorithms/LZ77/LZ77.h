#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
using namespace std;

class LZ77
{
public:
	vector<uint8_t> encode(vector<uint8_t>& string, int b_size);
	vector<uint8_t> decode(vector<uint8_t>& string);
};

