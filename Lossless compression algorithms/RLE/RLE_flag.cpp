#include "RLE_flag.h"

vector<uint8_t> RLE_flag::encode(vector<uint8_t>& data) {
    vector<uint8_t> output;
    int length = data.size();
    for (int i = 0; i < length;) {
        if (i + 1 < length && data[i] == data[i + 1]) {
            uint8_t sym = data[i];
            uint8_t count = 1;
            while (count < 127 && i + count < length && data[i + count] == sym) {
                count++;
            }
            output.push_back(count);
            output.push_back(sym);
            i += count;
        }
        else {
            int start_pos = i;
            int count = 0;
            while (i < length && count < 127) {
                if (i + 1 < length && data[i] == data[i + 1]) {
                    break;
                }
                else {
                    count++;
                    i++;
                }
            }
            if (count > 0) {
                output.push_back(count | 0x80);
                output.insert(output.end(), data.begin() + start_pos, data.begin() + start_pos + count);
            }
        }
    }
    return output;
}

vector<uint8_t> RLE_flag::decode(vector<uint8_t>& data) {
    vector<uint8_t> output;
    int length = data.size();
    for (int i = 0; i < length;) {
        uint8_t byte = data[i++];
        uint8_t count = byte & 0x7F;
        if ((byte & 0x80) == 0x80) {
            output.insert(output.end(), data.begin() + i, data.begin() + i + count);
            i += count;
        }
        else {
            uint8_t s = data[i++];
            output.insert(output.end(), count, s);
        }
    }
    return output;
}