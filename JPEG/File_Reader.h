#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
using namespace std;

class File_Reader {
public:
    template<typename T>
    vector<T> read_file(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cout << "Cannot open file: " << filename << endl;
            return {};
        }
        file.seekg(0, ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, ios::beg);
        vector<T> buffer(file_size / sizeof(T));
        file.read(reinterpret_cast<char*>(buffer.data()), file_size);
        return buffer;
    }

    template<typename T>
    void vector_to_file(const vector<T>& data, const string& filename) {
        ofstream file(filename, ios::binary);
        if (!file) {
            cout << "Cannot open file for writing: " << filename << endl;
            return;
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
        if (!file) {
            cout << "Error writing to file: " << filename << endl;
        }
    }
};