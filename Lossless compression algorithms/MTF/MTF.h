#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <queue>
#include <algorithm>
using namespace std;

class MTF
{
private:
	uint64_t read_bytes(vector<uint8_t>& data, int i, int ms);
	void write_bytes(vector<uint8_t>& data, uint64_t sym, int ms);
public:
	vector<uint8_t> encode(vector<uint8_t>& data, int ms);
	vector<uint8_t> decode(vector<uint8_t>& data, int ms);
};