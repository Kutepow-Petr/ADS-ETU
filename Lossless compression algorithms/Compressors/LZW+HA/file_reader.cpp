#include "file_reader.h"

vector<uint8_t> read_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Cannot open file: " << filename << endl;
    }
    vector<uint8_t> buffer;
    const size_t chunk_size = 4096;
    vector<char> chunk(chunk_size);
    while (file.read(chunk.data(), chunk_size) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        buffer.insert(buffer.end(),
            reinterpret_cast<uint8_t*>(chunk.data()),
            reinterpret_cast<uint8_t*>(chunk.data()) + bytes_read);
    }
    return buffer;
}

void vector_to_file(const vector<uint8_t>& data, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cout << "Cannot open file for writing: " << filename << endl;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file) {
        cout << "Error writing to file: " << filename << endl;
    }
}