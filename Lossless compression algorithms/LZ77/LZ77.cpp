#include "LZ77.h"

vector<uint8_t> LZ77::encode(vector<uint8_t>& string, int b_size) {
    vector<uint8_t> res;
    int i = 0;
    while (i < string.size()) {
        uint8_t offset = 0;
        uint8_t length = 0;
        uint8_t next_char;
        for (int j = max(0, i - b_size); j < i; j++) {
            int cur_len = 0;
            while (i + cur_len < string.size() - 1 && j + cur_len < i && string[j + cur_len] == string[i + cur_len]) {
                cur_len++;
            }
            if (length < cur_len) {
                length = cur_len;
                offset = i - j;
            }
        }
        next_char = string[i + length];
        res.push_back(offset);
        res.push_back(length);
        res.push_back(next_char);
        i += length + 1;
    }
    return res;
}

vector<uint8_t> LZ77::decode(vector<uint8_t>& string) {
    vector<uint8_t> res;
    int i = 0;
    while (i < string.size()) {
        uint8_t offset = string[i];
        uint8_t length = string[i + 1];
        uint8_t next_char = string[i + 2];
        int start = res.size() - offset;
        for (int j = 0; j < length; j++) {
            res.push_back(res[start + j]);
        }
        if (next_char != 0) {
            res.push_back(next_char);
        }
        i += 3;
    }
    return res;
}