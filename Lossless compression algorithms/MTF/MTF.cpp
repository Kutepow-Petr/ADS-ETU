#include "MTF.h"

uint64_t MTF::read_bytes(vector<uint8_t>& data, int i, int ms) {
    uint64_t sym = 0;
    for (int j = 0; j < ms; j++) {
        sym = (sym << 8) | data[i + j];
    }
    return sym;
}

void MTF::write_bytes(vector<uint8_t>& data, uint64_t sym, int ms) {
    for (int j = 0; j < ms; j++) {
        int shift = (ms - 1 - j) * 8;
        data.push_back((sym >> shift) & 0xFF);
    }
}

vector<uint8_t> MTF::encode(vector<uint8_t>& data, int ms) {
    uint64_t alphabet_size = pow(2, 8 * ms);
    vector<uint8_t> dict;
    for (uint64_t i = 0; i < alphabet_size; i++) {
        write_bytes(dict, i, ms);
    }
    vector<uint8_t> output;
    for (int j = 0; j < data.size(); j += ms) {
        uint64_t sym = read_bytes(data, j, ms);
        auto it = find(dict.begin(), dict.end(), sym);
        uint64_t i = distance(dict.begin(), it);
        write_bytes(output, i, ms);
        dict.insert(dict.begin(), dict[i]);
        dict.erase(dict.begin() + i + 1);
    }
    return output;
}

vector<uint8_t> MTF::decode(vector<uint8_t>& data, int ms) {
    uint64_t alphabet_size = pow(2, 8 * ms);
    vector<uint8_t> dict;
    for (uint64_t i = 0; i < alphabet_size; i++) {
        write_bytes(dict, i, ms);
    }
    vector<uint8_t> output;
    for (int j = 0; j < data.size(); j += ms) {
        uint64_t i = read_bytes(data, j, ms);
        write_bytes(output, dict[i], ms);
        dict.insert(dict.begin(), dict[i]);
        dict.erase(dict.begin() + i + 1);
    }
    return output;
}