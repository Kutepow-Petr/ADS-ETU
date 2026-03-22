#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <queue>
#include <algorithm>
using namespace std;

class BWT
{
private:
    struct Matrix {
        vector<vector<uint8_t>> matrix;

        Matrix(vector<uint8_t> data, int i, int n);
        Matrix(int n) { matrix = vector<vector<uint8_t>>(n); }

    private:
        void counting_sort();
    };
public:
    void encode_matrix(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n);
    void encode_display(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n);
    void decode_matrix(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n);
    void decode_display(const vector<uint8_t>& data, vector<uint8_t>& output, int i, int n);
    vector<uint8_t> write_blocks_matrix(vector<uint8_t>& data, uint64_t block_size);
    vector<uint8_t> write_blocks_display(vector<uint8_t>& data, uint64_t block_size);
    vector<uint8_t> read_blocks_matrix(vector<uint8_t>& data);
    vector<uint8_t> read_blocks_display(vector<uint8_t>&data);
};