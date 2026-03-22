#include "Huffman.h"

void Huffman::Node::generate_codes(map<uint8_t, vector<bool>>& code_dict, vector<bool> code = {}) {
    if (left == NULL && right == NULL) {
        code_dict[sym] = code;
        return;
    }
    if (left != NULL) {
        code.push_back(0);
        left->generate_codes(code_dict, code);
        code.pop_back();
    }
    if (right != NULL) {
        code.push_back(1);
        right->generate_codes(code_dict, code);
        code.pop_back();
    }
}

Huffman::Tree::Tree(map<uint8_t, float> symbols_freq_dict) {
    vector<Node*> nodes;
    for (auto& [sym, freq] : symbols_freq_dict) {
        nodes.push_back(new Node(sym, freq));
    }
    Node* left;
    Node* right;
    Node* parent;
    while (nodes.size() > 1) {
        sort(
            nodes.begin(),
            nodes.end(), [](Node* a, Node* b) {
                return a->freq < b->freq;
            }
        );
        left = nodes[0];
        right = nodes[1];
        nodes.erase(nodes.begin());
        nodes.erase(nodes.begin());
        parent = new Node(left->freq + right->freq, left, right);
        nodes.push_back(parent);
    }
    this->root = nodes[0];
}

Huffman::Tree::Tree(vector<uint8_t> data) {
    int i = 0;
    int dict_length = (data[i] << 8) | (data[i + 1]);
    i += 2;
    vector<Node*> nodes;
    int tree_height = 0;

    for (int j = 0; j < dict_length; j++) {
        uint8_t sym = data[i++];
        float code_len = data[i++];
        nodes.push_back(new Node(sym, code_len));
        if (tree_height < code_len) {
            tree_height = code_len;
        }
    }

    Node* left;
    Node* right;
    while (nodes.size() > 1) {
        for (int j = 0; j < nodes.size() - 1; j++) {
            left = nodes[j];
            right = nodes[j + 1];

            if (left->freq == tree_height && right->freq == tree_height) {
                Node* parent = new Node(tree_height - 1, left, right);
                nodes[j] = parent;
                nodes.erase(nodes.begin() + j + 1);
            }
        }
        tree_height--;
    }
    root = nodes[0];
}

map<uint8_t, float> Huffman::symbol_frequencies(vector<uint8_t>& data) {
    map<uint8_t, float> dict;
    int length = data.size();
    for (int i = 0; i < data.size(); i++) {
        dict[data[i]]++;
    }
    for (auto& [symbol, freq] : dict) {
        freq /= length;
    }
    return dict;
}

void Huffman::Tree::save(vector<uint8_t>& output, map<uint8_t, vector<bool>> symbols_freq_dict, Node* curr = NULL) {
    if (curr == NULL) {
        int codes_size = symbols_freq_dict.size();
        output.push_back(codes_size >> 8);
        output.push_back(codes_size & 0xFF);
        curr = root;
    }
    if (curr->left == NULL && curr->right == NULL) {
        uint8_t sym = curr->sym;
        output.push_back(sym);
        output.push_back(symbols_freq_dict[sym].size());
        return;
    }
    if (curr->left != NULL) {
        save(output, symbols_freq_dict, curr->left);
    }
    if (curr->right != NULL) {
        save(output, symbols_freq_dict, curr->right);
    }
}

vector<uint8_t> Huffman::encode(vector<uint8_t>& data) {
    Tree haffman_tree = Tree(symbol_frequencies(data));

    map<uint8_t, vector<bool>> symbols_freq_dict;
    haffman_tree.root->generate_codes(symbols_freq_dict);

    vector<uint8_t> output;
    haffman_tree.save(output, symbols_freq_dict);

    uint8_t byte = 0x00;
    int bits_count = 7;
    vector<bool> code;
    for (uint8_t sym : data) {
        code = symbols_freq_dict[sym];
        for (bool digit : code) {
            if (bits_count < 0) {
                output.push_back(byte);
                byte = 0x00;
                bits_count = 7;
            }
            byte |= (digit << bits_count--);
        }
    }
    output.push_back(byte);
    output.push_back(7 - bits_count);

    return output;
}

vector<uint8_t> Huffman::decode(vector<uint8_t> data) {
    Tree haffman_tree = Tree(data);
    int i = ((data[0] << 8) | (data[1])) * 2 + 2;
    vector<uint8_t> output;

    Node* curr = haffman_tree.root;
    uint8_t byte;
    for (int; i < data.size() - 2; i++) {
        byte = data[i];
        for (int bit = 7; bit >= 0; bit--) {
            bool digit = (byte >> bit) & 1;
            if (digit) {
                curr = curr->right;
            }
            else {
                curr = curr->left;
            }
            if (curr->left == NULL && curr->right == NULL) {
                output.push_back(curr->sym);
                curr = haffman_tree.root;
            }
        }
    }

    byte = data[i];
    int num_last_bits = data[data.size() - 1];
    for (int bit = 7; bit >= 8 - num_last_bits; bit--) {
        bool digit = (byte >> bit) & 1;
        if (digit) {
            curr = curr->right;
        }
        else {
            curr = curr->left;
        }
        if (curr->left == NULL && curr->right == NULL) {
            output.push_back(curr->sym);
            curr = haffman_tree.root;
        }
    }
    return output;
}