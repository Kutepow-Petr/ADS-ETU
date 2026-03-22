#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
using namespace std;

vector<uint8_t> read_file(const string& filename);
void vector_to_file(const vector<uint8_t>& data, const string& filename);