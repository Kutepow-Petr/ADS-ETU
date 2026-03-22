#pragma once
#include <vector>
#include <cstdint>
using namespace std;

class RLE_flag
{
public:
	vector<uint8_t> encode(vector<uint8_t>& data);
	vector<uint8_t> decode(vector<uint8_t>& data);
};