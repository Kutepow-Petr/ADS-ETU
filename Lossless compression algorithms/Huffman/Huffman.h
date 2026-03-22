#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
using namespace std;

class Huffman
{
private:
    struct Node {
        uint8_t sym;
        float freq;
        Node* left;
        Node* right;

        Node(uint8_t sym, float freq) : sym(sym), freq(freq), left(NULL), right(NULL) {}
        Node(float freq, Node* left, Node* right) : freq(freq), left(left), right(right) {}
        ~Node() { delete left; delete right; }

        void generate_codes(map<uint8_t, vector<bool>>& code_dict, vector<bool> code);
    };

    struct Tree {
        Node* root;

        static bool compare(const Node* a, const Node* b) { return a->freq < b->freq; }

        Tree(map<uint8_t, float> symbols_freq_dict);
        Tree(vector<uint8_t> data);

        ~Tree() { delete root; }

        void save(vector<uint8_t>& output, map<uint8_t, vector<bool>> symbols_freq_dict, Node* curr);
    };

    map<uint8_t, float> symbol_frequencies(vector<uint8_t>& data);
public:
    vector<uint8_t> encode(vector<uint8_t>& data);
    vector<uint8_t> decode(vector<uint8_t> data);
};