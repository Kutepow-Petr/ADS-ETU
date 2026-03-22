#include "LZW.h"

vector<uint8_t> LZW::encode(vector<uint8_t>& data, uint16_t buffer_size) {
    vector<uint8_t> output;
    map<vector<uint8_t>, uint16_t> dict;
    for (int i = 0; i < 256; i++) {
        dict[{static_cast<uint8_t>(i)}] = i;
    }
    vector<uint8_t> curr;
    uint16_t next_code = 256;
    buffer_size = min(int(buffer_size), 65535);
    for (uint8_t sym : data) {
        vector<uint8_t> test = curr;
        test.push_back(sym);
        auto it = dict.find(test);
        if (it != dict.end()) {
            curr = test;
        }
        else {
            if (!curr.empty()) {
                uint16_t code = dict[curr];
                output.push_back((code >> 8) & 0xFF);
                output.push_back(code & 0xFF);
            }
            if (next_code < buffer_size) {
                dict[test] = next_code++;
            }
            curr = { sym };
        }
    }
    if (!curr.empty()) {
        uint16_t code = dict[curr];
        output.push_back((code >> 8) & 0xFF);
        output.push_back(code & 0xFF);
    }
    return output;
}

vector<uint8_t> LZW::decode(vector<uint8_t>& data) {
    vector<uint8_t> output;
    vector<vector<uint8_t>> dict;
    for (int i = 0; i < 256; i++) {
        dict.push_back({ static_cast<uint8_t>(i) });
    }

    int j = 0;
    uint16_t prev_code = (data[j] << 8) | data[j + 1];
    j += 2;
    vector<uint8_t> prev_entry = dict[prev_code];
    output.insert(output.end(), prev_entry.begin(), prev_entry.end());

    for (; j < data.size(); j += 2) {
        uint16_t code = (data[j] << 8) | data[j + 1];
        vector<uint8_t> entry;
        if (code < dict.size()) {
            entry = dict[code];
        }
        else if (code == dict.size()) {
            entry = prev_entry;
            entry.push_back(prev_entry[0]);
        }
        for (uint8_t s : entry) {
            output.push_back(s);
        }
        if (!prev_entry.empty() && !entry.empty()) {
            vector<uint8_t> new_entry = prev_entry;
            new_entry.push_back(entry[0]);
            dict.push_back(new_entry);
        }
        prev_entry = entry;
    }
    return output;
}