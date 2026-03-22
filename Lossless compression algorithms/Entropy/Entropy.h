#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
using namespace std;

class Entropy
{
private:
	uint64_t read_bytes(vector<uint8_t>& data, int i, int ms);
	map<uint64_t, unsigned> symbols_counting(vector<uint8_t>& data, int ms);
public:
	double calc(vector<uint8_t>& data, int ms);
};