#include "BWT.h"

BWT::Matrix::Matrix(vector<uint8_t> data, int i, int n) {
    for (int j = 0; j < n; j++) {
        vector<uint8_t> rotation;
        for (size_t k = 0; k < n; k++) {
            rotation.push_back(data[i + ((j + k) % n)]);
        }
        matrix.push_back(rotation);
    }
    counting_sort();
}

void BWT::Matrix::counting_sort() {
    int n = matrix.size();
    if (n == 0) return;
    vector<vector<uint8_t>> res(n);
    for (int pos = n - 1; pos >= 0; pos--) {
        int count[256] = { 0 };
        for (const auto& str : matrix) {
            count[str[pos]]++;
        }
        for (int j = 1; j < 256; j++) {
            count[j] += count[j - 1];
        }
        for (int j = n - 1; j >= 0; j--) {
            uint8_t s = matrix[j][pos];
            int idx = count[s] - 1;
            res[idx] = matrix[j];
            count[s]--;
        }
        matrix = res;
        fill(res.begin(), res.end(), vector<uint8_t>());
    }
}

void BWT::encode_matrix(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n) {
    Matrix matrix = Matrix(data, i, n);

    for (int j = 0; j < n; j++) {
        output.push_back(matrix.matrix[j][n - 1]);
    }

    vector<uint8_t> target(data.begin() + i, data.begin() + i + n);
    auto it = find(matrix.matrix.begin(), matrix.matrix.end(), target);
    int target_str = distance(matrix.matrix.begin(), it);

    for (int j = 0; j < 4; j++) {
        output.push_back((target_str >> (j * 8)) & 0xFF);
    }
}

bool compare(int a, int b, const vector<uint8_t>& data, int i, int n) {
    for (int k = 0; k < n; k++) {
        uint8_t ca = data[i + (a + k) % n];
        uint8_t cb = data[i + (b + k) % n];
        if (ca != cb) return ca < cb;
    }
    return false;
}

void BWT::encode_display(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n) {
    vector<int> suffix_array(n);
    for (int j = 0; j < n; j++) {
        suffix_array[j] = j;
    }
    sort(suffix_array.begin(), suffix_array.end(),
        [&](int a, int b) { return compare(a, b, data, i, n); });
    vector<uint8_t> last_column(n);
    for (int j = 0; j < n; j++) {
        last_column[j] = data[i + ((suffix_array[j] + n - 1) % n)];
    }
    output.insert(output.end(), last_column.begin(), last_column.end());
    int target_str = 0;
    for (int j = 0; j < n; j++) {
        if (suffix_array[j] == 0) {
            target_str = j;
            break;
        }
    }
    for (int j = 0; j < 4; j++) {
        output.push_back((target_str >> (j * 8)) & 0xFF);
    }
}

void BWT::decode_matrix(vector<uint8_t>& data, vector<uint8_t>& output, int i, int n) {
    int target_str = 0;
    for (int j = 0; j < 4; j++) {
        target_str |= (data[i + n + j] << (j * 8));
    }
    vector<uint8_t> last_column;
    for (int j = 0; j < n; j++) {
        last_column.push_back(data[i + j]);
    }

    vector<uint8_t> first_column = last_column;
    sort(first_column.begin(), first_column.end());
    Matrix matrix = Matrix(n);
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
            matrix.matrix[k].insert(matrix.matrix[k].begin(), last_column[k]);
        }
        sort(matrix.matrix.begin(), matrix.matrix.end());
    }

    output.insert(output.end(), matrix.matrix[target_str].begin(), matrix.matrix[target_str].end());
}

void BWT::decode_display(const vector<uint8_t>& data, vector<uint8_t>& output, int i, int n) {
    int target_str = 0;
    for (int j = 0; j < 4; j++) {
        target_str |= (data[i + n + j] << (j * 8));
    }

    vector<uint8_t> last_column;
    for (int j = 0; j < n; j++) {
        last_column.push_back(data[i + j]);
    }
    vector<uint8_t> first_column = last_column;
    sort(first_column.begin(), first_column.end());
    unordered_map<uint8_t, queue<int>> positions;
    for (int j = 0; j < n; j++) {
        positions[last_column[j]].push(j);
    }
    vector<int> display_first_last(n);
    for (int j = 0; j < n; j++) {
        display_first_last[j] = positions[first_column[j]].front();
        positions[first_column[j]].pop();
    }

    int row = target_str;
    for (int j = 0; j < n; j++) {
        output.push_back(first_column[row]);
        row = display_first_last[row];
    }
}

vector<uint8_t> BWT::write_blocks_matrix(vector<uint8_t>& data, uint64_t block_size) {
    vector<uint8_t> output;
    uint64_t real_block_size;
    for (int i = 0; i < data.size(); i += block_size) {
        real_block_size = min(block_size, data.size() - i);
        for (int j = 0; j < 4; j++) {
            output.push_back((real_block_size >> (j * 8)) & 0xFF);
        }
        encode_matrix(data, output, i, real_block_size);
    }
    return output;
}

vector<uint8_t> BWT::write_blocks_display(vector<uint8_t>& data, uint64_t block_size) {
    vector<uint8_t> output;
    uint64_t real_block_size;
    for (int i = 0; i < data.size(); i += block_size) {
        real_block_size = min(block_size, data.size() - i);
        for (int j = 0; j < 4; j++) {
            output.push_back((real_block_size >> (j * 8)) & 0xFF);
        }
        encode_display(data, output, i, real_block_size);
    }
    return output;
}

vector<uint8_t> BWT::read_blocks_matrix(vector<uint8_t>& data) {
    vector<uint8_t> output;
    uint64_t block_size = 0;
    for (int i = 0; i < data.size(); i += block_size + 8) {
        block_size = 0;
        for (int j = 0; j < 4; j++) {
            block_size |= (data[i + j] << (j * 8));
        }
        decode_matrix(data, output, i + 4, block_size);
    }
    return output;
}

vector<uint8_t> BWT::read_blocks_display(vector<uint8_t>& data) {
    vector<uint8_t> output;
    uint64_t block_size = 0;
    for (int i = 0; i < data.size(); i += block_size + 8) {
        block_size = 0;
        for (int j = 0; j < 4; j++) {
            block_size |= (data[i + j] << (j * 8));
        }
        decode_display(data, output, i + 4, block_size);
    }
    return output;
}