#include "RLE_group.h"

uint64_t RLE_group::read_bytes(vector<uint8_t>& data, int i, int ms) {
    uint64_t sym = 0;
    for (int j = 0; j < ms; j++) {
        sym = (sym << 8) | data[i + j];
    }
    return sym;
}

void RLE_group::write_bytes(vector<uint8_t>& string, uint64_t sym, int ms) {
    for (int j = 0; j < ms; j++) {
        int shift = (ms - 1 - j) * 8;
        string.push_back((sym >> shift) & 0xFF);
    }
}



vector<uint8_t> RLE_group::encode(vector<uint8_t>& string, int ms, int mc) {
    vector<uint8_t> output;
    int i = 0;
    int length = string.size();
    while (i < length) {
        uint64_t sym = read_bytes(string, i, ms);
        uint64_t count = 1;
        uint64_t max_count = min((1ULL << 8 * mc) - 1, static_cast<unsigned long long>((length - i) / ms));
        while (count < max_count) {
            int next_pos = i + count * ms;
            if (next_pos + ms > length) {
                break;
            }
            uint64_t next_sym = read_bytes(string, next_pos, ms);
            if (next_sym != sym) {
                break;
            }
            count++;
        }
        write_bytes(output, count, mc);
        write_bytes(output, sym, ms);
        i += count * ms;
    }
    return output;
}

vector<uint8_t> RLE_group::decode(vector<uint8_t>& string, int ms, int mc) {
    vector<uint8_t> output;
    int i = 0;
    int length = string.size();
    while (i < length) {
        uint64_t count = read_bytes(string, i, mc);
        i += mc;
        uint64_t sym = read_bytes(string, i, ms);
        i += ms;
        for (uint64_t j = 0; j < count; j++) {
            write_bytes(output, sym, ms);
        }
    }
    return output;
}