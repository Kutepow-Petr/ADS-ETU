#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
using namespace std;

class LZSS
{
public:
	vector<uint8_t> encode(vector<uint8_t>& data, int buffer_size);
	vector<uint8_t> decode(vector<uint8_t>& data);
};