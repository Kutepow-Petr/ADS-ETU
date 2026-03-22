#include <iostream>
#include "BWT.h"
#include "MTF.h"
#include "Huffman.h"
#include "file_reader.h"
using namespace std;

int main(int argc, char* argv[]) {
    BWT bwt;
    MTF mtf;
    Huffman huffman;
    string file_path;
    cout << "Enter file path: ";
    cin >> file_path;
    int ms, mc, block_size;
    cout << "Enter block size for BWT: ";
    cin >> block_size;
    cout << "Enter the number of byte for characters for MTF: ";
    cin >> ms;
    file_path.erase(remove(file_path.begin(), file_path.end(), '\"'), file_path.end());
    int dot_pos = file_path.find_last_of('.');
    int slash_pos = file_path.find_last_of('\\') + 1;
    string file_name = file_path.substr(slash_pos, dot_pos - slash_pos);
    string extension = file_path.substr(dot_pos);
    vector<uint8_t> original = read_file(file_path);

    vector<uint8_t> encode = bwt.write_blocks_display(original, block_size);
    encode = mtf.encode(encode, ms);
    encode = huffman.encode(encode);
    vector_to_file(encode, "./results/" + file_name + "_encode.bin");

    vector<uint8_t> decode = huffman.decode(encode);
    decode = mtf.decode(decode, ms);
    decode = bwt.read_blocks_display(decode);
    vector_to_file(decode, "./results/" + file_name + "_decode" + extension);
    return 0;
}