#include "LZ78.h"

vector<uint8_t> LZ78::encode(vector<uint8_t>& string) {
    vector<uint8_t> res;
    map<vector<uint8_t>, uint8_t> dict;
    vector<uint8_t> curr;
    for (uint8_t s : string) {
        vector<uint8_t> test = curr;
        test.push_back(s);
        auto it = dict.find(test);
        if (it != dict.end()) {
            curr = test;
        }
        else {
            uint8_t i = 0;
            if (!curr.empty()) {
                i = dict[curr];
            }
            res.push_back(i);
            res.push_back(s);
            if (dict.size() < 256) {
                dict[test] = dict.size() + 1;
            }
            curr = {};
        }
    }
    if (!curr.empty()) {
        uint8_t i = dict[curr];
        res.push_back(i);
    }
    return res;
}

vector<uint8_t> LZ78::decode(vector<uint8_t>& string) {
    vector<uint8_t> res;
    vector<vector<uint8_t>> dict;
    dict.push_back({});
    int j = 0;
    while (j < string.size()) {
        uint8_t i = string[j++];
        uint8_t s = string[j++];
        vector<uint8_t> entry;
        if (i < dict.size()) {
            entry = dict[i];
        }
        entry.push_back(s);
        res.insert(res.end(), entry.begin(), entry.end());
        if (dict.size() < 256 && !entry.empty()) {
            dict.push_back(entry);
        }
    }
    return res;
}