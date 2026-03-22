#pragma once
#include <vector>
#include <cstdint>
using namespace std;

class RLE_group
{
private:
	uint64_t read_bytes(vector<uint8_t>& data, int i, int ms);
	void write_bytes(vector<uint8_t>& string, uint64_t sym, int ms);
public:
	vector<uint8_t> encode(vector<uint8_t>& string, int ms, int mc);
	vector<uint8_t> decode(vector<uint8_t>& string, int ms, int mc);
};