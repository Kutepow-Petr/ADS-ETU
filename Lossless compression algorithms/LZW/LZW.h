#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
using namespace std;

class LZW
{
public:
	vector<uint8_t> decode(vector<uint8_t>& data);
	vector<uint8_t> encode(vector<uint8_t>& data, uint16_t buffer_size);
};