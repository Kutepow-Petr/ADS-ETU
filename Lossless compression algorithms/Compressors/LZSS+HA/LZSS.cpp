#include "LZSS.h"

vector<uint8_t> LZSS::encode(vector<uint8_t>& data, int buffer_size) {
    vector<uint8_t> output;
    int i = 0;
    while (i < data.size()) {
        uint8_t flag = 0;
        vector<uint8_t> block;
        for (int bit = 0; bit < 8 && i < data.size(); bit++) {
            uint16_t offset = 0;
            uint16_t length = 0;
            uint16_t max_search = min(int(data.size() - i), 65535);
            for (int j = max(0, i - buffer_size); j < i; j++) {
                int curr_len = 0;
                while (curr_len < max_search && j + curr_len < i && data[j + curr_len] == data[i + curr_len]) {
                    curr_len++;
                }
                if (length < curr_len) {
                    length = curr_len;
                    offset = i - j;
                }
            }
            if (length > 4) {
                flag = flag | (1 << bit);
                block.push_back(offset >> 8);
                block.push_back(offset & 0xFF);
                block.push_back(length >> 8);
                block.push_back(length & 0xFF);
                i += length;
            }
            else {
                block.push_back(data[i]);
                i++;
            }
        }
        output.push_back(flag);
        output.insert(output.end(), block.begin(), block.end());
    }
    return output;
}

vector<uint8_t> LZSS::decode(vector<uint8_t>& data) {
    vector<uint8_t> output;
    int i = 0;
    while (i < data.size()) {
        uint8_t flag_byte = data[i++];
        for (int bit = 0; bit < 8 && i < data.size(); bit++) {
            if ((flag_byte >> bit) & 1) {
                if (i + 1 >= data.size()) {
                    break;
                }
                uint16_t offset = (data[i] << 8) | (data[i + 1]);
                i += 2;
                uint16_t length = (data[i] << 8) | (data[i + 1]);
                i += 2;
                int start = output.size() - offset;
                for (int j = 0; j < length; j++) {
                    output.push_back(output[start + j]);
                }
            }
            else {
                output.push_back(data[i++]);
            }
        }
    }
    return output;
}