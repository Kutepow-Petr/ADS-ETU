#include "Entropy.h"

uint64_t Entropy::read_bytes(vector<uint8_t>& data, int i, int ms) {
    uint64_t sym = 0;
    for (int j = 0; j < ms; j++) {
        sym = (sym << 8) | data[i + j];
    }
    return sym;
}

map<uint64_t, unsigned> Entropy::symbols_counting(vector<uint8_t>& data, int ms) {
    map<uint64_t, unsigned> dict;
    for (int i = 0; i < data.size(); i += ms) {
        uint64_t sym = read_bytes(data, i, ms);
        if (dict.count(sym) > 0) {
            dict[sym]++;
        }
        else {
            dict[sym] = 1;
        }
    }
    return dict;
}

double Entropy::calc(vector<uint8_t>& data, int ms) {
    map<uint64_t, unsigned> dict = symbols_counting(data, ms);
    double result = 0;
    double length = data.size() / ms;
    for (auto& [sym, count] : dict) {
        double p = double(count) / length;
        result += p * log2(p);
    }
    return -result;
}