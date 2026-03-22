#include "RLE.h"

vector<uint8_t> RLE::encode(vector<uint8_t>& data) {
    vector<uint8_t> output;
    int length = data.size();
    for (int i = 0; i < length;) {
        uint8_t s = data[i];
        uint8_t count = 1;
        while (count < 255 && i + count < length && data[i + count] == s) {
            count++;
        }
        output.push_back(count);
        output.push_back(s);
        i += count;
    }
    return output;
}

vector<uint8_t> RLE::decode(const vector<uint8_t>& data) {
    vector<uint8_t> output;
    for (size_t i = 0; i < data.size(); i += 2) {
        uint8_t count = data[i];
        uint8_t sym = data[i + 1];
        output.insert(output.end(), count, sym);
    }
    return output;
}