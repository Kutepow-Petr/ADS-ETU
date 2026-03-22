#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
using namespace std;


class Algorithmic_coding
{
private:
	union double_int {
		double dl;
		uint64_t nt;
	};

	struct range_s {
		double low;
		double high;
	};

	double read_double(vector<uint8_t> string, int i);
	void write_double(vector<uint8_t>& string, double v);
	unordered_map<uint8_t, double> symbols_p(vector<uint8_t>& string);
public:
	vector<uint8_t> encode(vector<uint8_t>& string);
	vector<uint8_t> decode(vector<uint8_t>& string);
	void print_encode_string(vector<uint8_t> string);
};