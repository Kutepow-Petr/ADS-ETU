#include <iostream>
#include "LZW.h"
#include "file_reader.h"
using namespace std;

int main(int argc, char* argv[]) {
    LZW lzw;
    string file_path;
    uint16_t buffer_size;
    cout << "Enter file path: ";
    cin >> file_path;
    cout << "Enter buffer size: ";
    cin >> buffer_size;
    file_path.erase(remove(file_path.begin(), file_path.end(), '\"'), file_path.end());
    int dot_pos = file_path.find_last_of('.');
    int slash_pos = file_path.find_last_of('\\') + 1;
    string file_name = file_path.substr(slash_pos, dot_pos - slash_pos);
    string extension = file_path.substr(dot_pos);
    vector<uint8_t> original = read_file(file_path);

    vector<uint8_t> encode = lzw.encode(original, buffer_size);
    vector_to_file(encode, "./results/" + file_name + "_encode.bin");

    vector<uint8_t> decode = lzw.decode(encode);
    vector_to_file(decode, "./results/" + file_name + "_decode" + extension);
    return 0;
}