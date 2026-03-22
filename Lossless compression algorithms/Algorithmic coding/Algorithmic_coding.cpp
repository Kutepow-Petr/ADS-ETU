#include "Algorithmic_coding.h"

double Algorithmic_coding::read_double(vector<uint8_t> string, int i) {
    double_int res;
    res.nt = 0;
    for (int j = 0; j < 8; j++) {
        res.nt = (res.nt << 8) | string[i + j];
    }
    return res.dl;
}

void Algorithmic_coding::write_double(vector<uint8_t>& string, double v) {
    double_int number;
    number.dl = v;
    for (int i = 0; i < 8; i++) {
        int shift = (7 - i) * 8;
        string.push_back((number.nt >> shift) & 0xFF);
    }
}

unordered_map<uint8_t, double> Algorithmic_coding::symbols_p(vector<uint8_t>& string) {
    unordered_map<uint8_t, double> dict;
    int length = string.size();
    for (int i = 0; i < string.size(); i++) {
        uint8_t s = string[i];
        if (dict.count(s) > 0) {
            dict[s]++;
        }
        else {
            dict[s] = 1.0;
        }
    }
    for (auto& [k, v] : dict) {
        v /= length;
    }
    return dict;
}

vector<uint8_t> Algorithmic_coding::encode(vector<uint8_t>& string) {
    unordered_map<uint8_t, double> sym_p = symbols_p(string);
    vector<uint8_t> res;
    res.push_back(sym_p.size());
    for (auto [s, p] : sym_p) {
        res.push_back(s);
        write_double(res, p);
    }

    unordered_map<uint8_t, range_s> ranges;
    double low = 0.0;
    for (auto [s, p] : sym_p) {
        double high = low + p;
        ranges[s] = { low, high };
        low = high;
    }

    low = 0.0;
    double high = 1.0;
    for (uint8_t s : string) {
        double r = high - low;
        double s_low = ranges[s].low;
        double s_high = ranges[s].high;
        high = low + r * s_high;
        low = low + r * s_low;
    }
    res.push_back(string.size());
    write_double(res, (high + low) / 2);
    return res;
}

vector<uint8_t> Algorithmic_coding::decode(vector<uint8_t>& string) {
    int i = 0;
    int dict_s = string[i++];
    unordered_map<uint8_t, range_s> sym_p;
    double low = 0.0;
    for (int j = 0; j < dict_s; j++) {
        uint8_t s = string[i++];
        double p = read_double(string, i);
        i += 8;
        double high = low + p;
        sym_p[s] = { low, high };
        low = high;
    }
    int length = string[i++];
    double encode = read_double(string, i);

    vector<uint8_t> res;
    low = 0.0;
    double high = 1.0;
    for (int j = 0; j < length; j++) {
        double r = high - low;
        double f = (encode - low) / r;
        uint8_t s;
        for (auto [k, v] : sym_p) {
            if (f > v.low && f < v.high) {
                s = k;
                break;
            }
        }
        res.push_back(s);
        high = low + r * sym_p[s].high;
        low = low + r * sym_p[s].low;
    }
    return res;
}

void Algorithmic_coding::print_encode_string(vector<uint8_t> string) {
    int i = 0;
    int dict_l = string[i++];
    cout << "0x" << hex << setw(2) << setfill('0') << +dict_l << " ";
    for (; i < string.size() - 9; i += 9) {
        cout << "0x" << hex << setw(2) << setfill('0') << +string[i] << " ";
        double p = read_double(string, i + 1);
        cout << p << " ";
    }
    cout << "0x" << hex << setw(2) << setfill('0') << +string[i++] << " ";
    cout << read_double(string, i) << " ";
    cout << endl;
}