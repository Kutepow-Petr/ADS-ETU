#pragma once
#include "Jpeg.h"

vector<Jpeg::YCbCr> Jpeg::rgb_to_ycbcr(const vector<Jpeg::RGB>& data) {
    vector<YCbCr> output;
    for (RGB rgb_pixel : data) {
        float r = rgb_pixel.r;
        float g = rgb_pixel.g;
        float b = rgb_pixel.b;
        YCbCr ycbcr_pixel;
        ycbcr_pixel.y = static_cast<uint8_t>(clamp(0.299f * r + 0.587f * g + 0.114f * b, 0.0f, 255.0f));
        ycbcr_pixel.cb = static_cast<uint8_t>(clamp(-0.1687f * r - 0.3313f * g + 0.5f * b + 128.0f, 0.0f, 255.0f));
        ycbcr_pixel.cr = static_cast<uint8_t>(clamp(0.5f * r - 0.4187f * g - 0.0813f * b + 128.0f, 0.0f, 255.0f));
        output.push_back(ycbcr_pixel);
    }
    return output;
}

vector<Jpeg::RGB> Jpeg::ycbcr_to_rgb(const vector<Jpeg::YCbCr>& data) {
    vector<RGB> output;
    for (const auto& ycbcr_pixel : data) {
        float y = ycbcr_pixel.y;
        float cb = ycbcr_pixel.cb - 128.0f;
        float cr = ycbcr_pixel.cr - 128.0f;
        RGB rgb_pixel;
        rgb_pixel.r = static_cast<uint8_t>(clamp(y + 1.402f * cr, 0.0f, 255.0f));
        rgb_pixel.g = static_cast<uint8_t>(clamp(y - 0.344f * cb - 0.714f * cr, 0.0f, 255.0f));
        rgb_pixel.b = static_cast<uint8_t>(clamp(y + 1.772f * cb, 0.0f, 255.0f));
        output.push_back(rgb_pixel);
    }
    return output;
}

void Jpeg::downsample(const vector<YCbCr>& data, vector<uint8_t>& res_y, vector<uint8_t>& res_cb, vector<uint8_t>& res_cr, uint16_t& width, uint16_t& height, uint8_t coeff) {
    uint16_t new_width = width / coeff;
    uint16_t new_height = height / coeff;
    vector<uint8_t> downsampled;
    uint16_t block_size = coeff * coeff;
    for (uint16_t y = 0; y < new_height; y++) {
        for (uint16_t x = 0; x < new_width; x++) {
            uint64_t sum_cb = 0;
            uint64_t sum_cr = 0;
            for (uint8_t dy = 0; dy < coeff; dy++) {
                for (uint8_t dx = 0; dx < coeff; dx++) {
                    uint16_t pos_x = x * coeff + dx;
                    uint16_t pos_y = y * coeff + dy;
                    YCbCr pixel = data[pos_y * width + pos_x];
                    sum_cb += pixel.cb;
                    sum_cr += pixel.cr;
                    res_y.push_back(pixel.y);
                }
            }
            res_cb.push_back(static_cast<uint8_t>(sum_cb / block_size));
            res_cr.push_back(static_cast<uint8_t>(sum_cr / block_size));
        }
    }
    width = new_width * coeff;
    height = new_height * coeff;
}

vector<Jpeg::YCbCr> Jpeg::upsample(const vector<uint8_t>& data_y,
    const vector<uint8_t>& data_cb,
    const vector<uint8_t>& data_cr,
    uint16_t width, uint16_t height,
    uint8_t coeff) {
    uint16_t blocks_per_row = width / coeff;
    uint16_t blocks_per_col = height / coeff;
    vector<YCbCr> upsampled(width * height);

    size_t y_idx = 0;
    size_t chroma_idx = 0;

    for (uint16_t block_y = 0; block_y < blocks_per_col; block_y++) {
        for (uint16_t block_x = 0; block_x < blocks_per_row; block_x++) {
            uint8_t cb = data_cb[chroma_idx];
            uint8_t cr = data_cr[chroma_idx];
            chroma_idx++;

            for (uint8_t dy = 0; dy < coeff; dy++) {
                for (uint8_t dx = 0; dx < coeff; dx++) {
                    uint16_t pos_x = block_x * coeff + dx;
                    uint16_t pos_y = block_y * coeff + dy;

                    YCbCr pixel;
                    pixel.y = data_y[y_idx++];
                    pixel.cb = cb;
                    pixel.cr = cr;

                    upsampled[pos_y * width + pos_x] = pixel;
                }
            }
        }
    }

    return upsampled;
}

vector<Jpeg::YCbCr> Jpeg::bilinear_upsample(const vector<uint8_t>& data_y, const vector<uint8_t>& data_cb, const vector<uint8_t>& data_cr, uint16_t width, uint16_t height, uint8_t coeff) {
    uint16_t blocks_per_row = width / coeff;
    uint16_t blocks_per_col = height / coeff;
    vector<YCbCr> upsampled(width * height);
    uint64_t i_y = 0;
    uint64_t i_cbcr = 0;
    pair<vector<uint8_t>, vector<uint8_t>> CB, CR;
    CB.second.push_back(data_cb[0]);
    CR.second.push_back(data_cr[0]);
    for (uint16_t j = 0; j < blocks_per_row; j++) {
        CB.second.push_back(data_cb[j]);
        CR.second.push_back(data_cr[j]);
    }
    for (uint16_t y = 0; y < blocks_per_col; y++) {
        CB.first = CB.second;
        CR.first = CR.second;
        CB.second[0] = data_cb[y * blocks_per_row];
        CR.second[0] = data_cr[y * blocks_per_row];
        for (uint16_t x = 0; x < blocks_per_row; x++) {
            CB.second[x + 1] = data_cb[i_cbcr];
            CR.second[x + 1] = data_cr[i_cbcr++];
            uint16_t x1 = x * coeff;
            uint16_t x2 = (x + 1) * coeff;
            uint16_t y1 = y * coeff;
            uint16_t y2 = (y + 1) * coeff;
            for (uint8_t dy = 0; dy < coeff; dy++) {
                for (uint8_t dx = 0; dx < coeff; dx++) {
                    YCbCr pixel;
                    pixel.y = data_y[i_y++];
                    uint16_t pos_x = x * coeff + dx;
                    uint16_t pos_y = y * coeff + dy;
                    pixel.cb = bilinear_interpolation(
                        { {x1, y1}, {x2, y1}, {x1, y2}, {x2, y2} },
                        {CB.first[x], CB.first[x + 1], CB.second[x], CB.second[x + 1]},
                        pos_x, pos_y);
                    pixel.cr = bilinear_interpolation(
                        { {x1, y1}, {x2, y1}, {x1, y2}, {x2, y2} },
                        { CR.first[x], CR.first[x + 1], CR.second[x], CR.second[x + 1] },
                        pos_x, pos_y);
                    upsampled[pos_y * width + pos_x] = pixel;
                }
            }
        }
    }
    return upsampled;
}

uint8_t Jpeg::linear_interpolation(uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2, uint16_t x) {
    float dy = y2 - y1;
    float dx = x2 - x1;
    float dt = x - x1;
    float result = y1 + (dy * dt / dx);
    return static_cast<uint8_t>(result + 0.5f);
}

uint8_t Jpeg::linear_spline(vector<uint16_t> X, vector<uint8_t> Y, uint16_t x) {
    auto it = lower_bound(X.begin(), X.end(), x);
    uint8_t i = distance(X.begin(), it) - 1;
    return linear_interpolation(X[i], X[i + 1], Y[i], Y[i + 1], x);
}

uint8_t Jpeg::bilinear_interpolation(vector<pair<uint16_t, uint16_t>> XY, vector<uint8_t> Z, uint16_t x, uint16_t y) {
    uint16_t x1 = XY[0].first;
    uint16_t x2 = XY[1].first;
    uint16_t y1 = XY[0].second;
    uint16_t y2 = XY[2].second;
    uint8_t f1 = linear_interpolation(x1, x2, Z[0], Z[1], x);
    uint8_t f2 = linear_interpolation(x1, x2, Z[2], Z[3], x);
    return linear_interpolation(y1, y2, f1, f2, y);
}

vector<Jpeg::Block> Jpeg::split_into_blocks(const vector<uint8_t>& data, uint16_t width, uint16_t height, uint8_t block_width, uint8_t block_height) {
    vector<Block> output;
    uint16_t blocks_per_row = ceil(float(width) / float(block_width));
    uint16_t blocks_per_col = ceil(float(height) / float(block_height));
    for (uint16_t block_y = 0; block_y < blocks_per_col; block_y++) {
        for (uint16_t block_x = 0; block_x < blocks_per_row; block_x++) {

            uint8_t real_width = min(static_cast<uint16_t>(block_width), static_cast<uint16_t>(width - block_x * block_width));
            uint8_t real_height = min(static_cast<uint16_t>(block_height), static_cast<uint16_t>(height - block_y * block_height));
            Block block(real_width, real_height);

            for (uint8_t y = 0; y < real_height; y++) {
                for (uint8_t x = 0; x < real_width; x++) {

                    uint16_t pos_y = block_y * block_height + y;
                    uint16_t pos_x = block_x * block_width + x;

                    block.data.push_back(data[pos_y * width + pos_x]);
                }

            }
            output.push_back(block);
        }
    }
    return output;
}

vector<uint8_t> Jpeg::blocks_to_vector(const vector<Jpeg::Block>& data, uint16_t width, uint16_t height) {
    vector<uint8_t> output(width * height);
    Block first_block = data[0];
    uint16_t blocks_per_row = ceil(float(width) / float(first_block.w));
    uint16_t blocks_per_col = ceil(float(height) / float(first_block.h));
    for (uint16_t block_y = 0; block_y < blocks_per_col; block_y++) {
        for (uint16_t block_x = 0; block_x < blocks_per_row; block_x++) {

            const Block& block = data[block_y * blocks_per_row + block_x];
            uint8_t block_width = block.w;
            uint8_t block_height = block.h;
            uint16_t pos_x = block_x * first_block.w;
            uint16_t pos_y = block_y * first_block.h;

            for (uint8_t y = 0; y < block_height; y++) {
                for (uint8_t x = 0; x < block_width; x++) {
                    uint16_t data_pos = y * block_width + x;
                    output[(pos_y + y) * width + (pos_x + x)] = block.data[data_pos];
                }
            }
        }
    }
    return output;
}

void Jpeg::DCT::forward_blocks(vector<Block>& data, bool fast) {
    uint64_t block_count = data.size();
    for (uint64_t block_i = 0; block_i < block_count; block_i++) {
        Block& block = data[block_i];
        uint8_t w = block.w;
        uint8_t h = block.h;
        if (fast) {
            if (w < 8 || h < 8) {
                Block new_block(8, 8);
                new_block.data.resize(64);
                uint64_t average = 0;
                for (uint8_t y = 0; y < h; y++) {
                    for (uint8_t x = 0; x < w; x++) {
                        new_block.data[y * 8 + x] = block.data[y * w + x];
                        average += block.data[y * w + x];
                    }
                }
                average /= w * h;
                for (uint8_t y = 0; y < 8; y++) {
                    for (uint8_t x = y < h ? w : 0; x < 8; x++) {
                        new_block.data[y * 8 + x] = average;
                    }
                }
                block = new_block;
            }
            forward_fast(block.data);
        }
        else {
            forward_basic(block.data, w, h);
        }
    }
}

void Jpeg::DCT::inverse_blocks(vector<Block>& data, bool fast) {
    uint64_t block_count = data.size();
    for (uint64_t block_i = 0; block_i < block_count; block_i++) {
        Block& block = data[block_i];
        if (fast) {
            inverse_fast(block.data);
        }
        else {
            uint8_t w = block.w;
            uint8_t h = block.h;
            inverse_basic(block.data, w, h);
        }
    }
}

void Jpeg::DCT::forward_basic(vector<int>& data, uint8_t m, uint8_t n) {
    vector<float> pixels(data.size());
    for (uint64_t i = 0; i < data.size(); i++) {
        pixels[i] = data[i] - 128.0;
    }
    for (int u = 0; u < m; u++) {
        for (int v = 0; v < n; v++) {
            float sum = 0.0;
            float cu = (u == 0) ? 0.7071 : 1.0;
            float cv = (v == 0) ? 0.7071 : 1.0;
            for (int x = 0; x < m; x++) {
                for (int y = 0; y < n; y++) {
                    float cos_x = cos(3.1416 * u * (2.0 * x + 1.0) / (2.0 * m));
                    float cos_y = cos(3.1416 * v * (2.0 * y + 1.0) / (2.0 * n));
                    sum += pixels[y * m + x] * cos_x * cos_y;
                }
            }
            data[v * m + u] = round(cu * cv * (2 / sqrt(m * n)) * sum);
        }
    }
}

void Jpeg::DCT::inverse_basic(vector<int>& data, uint8_t m, uint8_t n) {
    vector<float> pixels(data.size(), 0.0);
    for (int x = 0; x < m; x++) {
        for (int y = 0; y < n; y++) {
            float sum = 0.0;
            for (int u = 0; u < m; u++) {
                for (int v = 0; v < n; v++) {
                    float value = data[v * m + u];
                    float cu = (u == 0) ? 0.7071 : 1.0;
                    float cv = (v == 0) ? 0.7071 : 1.0;
                    float cos_x = cos(3.1416 * u * (2.0 * x + 1.0) / (2.0 * m));
                    float cos_y = cos(3.1416 * v * (2.0 * y + 1.0) / (2.0 * n));
                    sum += cu * cv * value * cos_x * cos_y;
                }
            }
            pixels[y * m + x] = round(sum * (2 / sqrt(m * n))) + 128.0;
        }
    }
    for (uint64_t i = 0; i < data.size(); i++) {
        data[i] = pixels[i];
    }
}

vector<float> Jpeg::DCT::multiply(const vector<float>& A, const vector<float>& B) {
    vector<float> C(64, 0.0f);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 8; k++) {
                sum += A[i * 8 + k] * B[k * 8 + j];
            }
            C[i * 8 + j] = sum;
        }
    }
    return C;
}

void Jpeg::DCT::forward_fast(vector<int>& data) {
    vector<float> f(data.size());
    for (uint16_t i = 0; i < data.size(); i++) {
        f[i] = static_cast<float>(data[i]) - 128.0f;
    }
    vector<float> temp = multiply(DCT_MATRIX, f);
    vector<float> F = multiply(temp, DCT_MATRIX_T);
    for (uint16_t i = 0; i < data.size(); i++) {
        data[i] = static_cast<int>(round(F[i]));
    }
}

void Jpeg::DCT::inverse_fast(vector<int>& data) {
    vector<float> F(data.size());
    for (uint16_t i = 0; i < data.size(); i++) {
        F[i] = static_cast<float>(data[i]);
    }
    vector<float> temp = multiply(DCT_MATRIX_T, F);
    vector<float> f = multiply(temp, DCT_MATRIX);
    for (uint16_t i = 0; i < data.size(); i++) {
        float pixel = f[i] + 128.0f;
        data[i] = static_cast<int>(clamp(round(pixel), 0.0f, 255.0f));
    }
}

Jpeg::Quantisator::Quantisator(uint8_t quality) {
    luminance.reserve(64);
    chrominance.reserve(64);
    int S = quality < 50 ? 5000 / quality : 200 - 2 * quality;
    for (uint8_t i = 0; i < 64; i++) {
        int value = ceil(LUMINANCE_QUANT_TABLE[i] * S / 100.0);
        luminance.push_back(max(1, value));
        value = ceil(CHROMINANCE_QUANT_TABLE[i] * S / 100.0);
        chrominance.push_back(max(1, value));
    }
}

void Jpeg::Quantisator::quant_coefficient(vector<Block>& data, bool isLuminance) {
    vector<uint8_t>& table = isLuminance ? luminance : chrominance;
    for (uint64_t block_i = 0; block_i < data.size(); block_i++) {
        Block& block = data[block_i];
        uint8_t w = block.w;
        uint8_t h = block.h;
        for (uint8_t i = 0; i < w * h; i++) {
            block.data[i] /= table[i];
        }
    }
}

void Jpeg::Quantisator::norm_coefficient(vector<Block>& data, bool isLuminance) {
    vector<uint8_t>& table = isLuminance ? luminance : chrominance;
    for (uint64_t block_i = 0; block_i < data.size(); block_i++) {
        Block& block = data[block_i];
        uint8_t w = block.w;
        uint8_t h = block.h;
        for (uint8_t i = 0; i < w * h; i++) {
            block.data[i] *= table[i];
        }
    }
}

void Jpeg::Block::zigzag_forward() {
    vector<int> transformed;
    uint8_t x = 0;
    uint8_t y = 0;
    transformed.push_back(data[y * w + x]);
    while (x < w - 1 || y < h - 1) {
        if (x < w - 1) {
            x++;
        }
        else {
            y++;
        }
        transformed.push_back(data[y * w + x]);
        while (x > 0 && y < h - 1) {
            x--;
            y++;
            transformed.push_back(data[y * w + x]);
        }
        if (y < h - 1) {
            y++;
        }
        else {
            x++;
        }
        transformed.push_back(data[y * w + x]);
        while (y > 0 && x < w - 1) {
            x++;
            y--;
            transformed.push_back(data[y * w + x]);
        }
    }
    data = transformed;
}

void Jpeg::Block::zigzag_inverse() {
    vector<int> destroyed(w * h);
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t i = 0;
    destroyed[y * w + x] = data[i++];
    while (x < w - 1 || y < h - 1) {
        if (x < w - 1) {
            x++;
        }
        else {
            y++;
        }
        destroyed[y * w + x] = data[i++];
        while (x > 0 && y < h - 1) {
            x--;
            y++;
            destroyed[y * w + x] = data[i++];
        }
        if (y < h - 1) {
            y++;
        }
        else {
            x++;
        }
        destroyed[y * w + x] = data[i++];
        while (y > 0 && x < w - 1) {
            x++;
            y--;
            destroyed[y * w + x] = data[i++];
        }
    }
    data = destroyed;
}

vector<int> Jpeg::VLC::DC_difference_encode(const vector<int>& data) {
    vector<int> differences{ data[0] };
    for (uint64_t i = 1; i < data.size(); i++) {
        differences.push_back(data[i] - data[i - 1]);
    }
    return differences;
}

vector<int> Jpeg::VLC::DC_difference_decode(const vector<int>& data) {
    vector<int> dc_coeffs{ data[0] };
    for (uint64_t i = 1; i < data.size(); i++) {
        dc_coeffs.push_back(dc_coeffs[i - 1] + data[i]);
    }
    return dc_coeffs;
}

template<typename Iterator>
vector <Jpeg::RLE_code> Jpeg::VLC::AC_RLE_encode(Iterator begin, Iterator end) {
    vector<RLE_code> rle_codes;
    int zero_count = 0;
    for (auto it = begin; it != end; it++) {
        int value = static_cast<int>(*it);
        if (value == 0) {
            zero_count++;
        }
        else {
            while (zero_count >= 16) {
                rle_codes.emplace_back(15, 0);
                zero_count -= 16;
            }
            rle_codes.emplace_back(zero_count, value);
            zero_count = 0;
        }
    }
    if (zero_count > 0) {
        rle_codes.emplace_back(0, 0);
    }
    else if (rle_codes.empty()) {
        rle_codes.emplace_back(0, 0);
    }
    return rle_codes;
}

vector<int> Jpeg::VLC::AC_RLE_decode(const vector<RLE_code>& data) {
    vector<int> ac_coeffs;
    for (const auto& code : data) {
        if (code.run == 0 && code.value == 0) {
            while (ac_coeffs.size() < 63) {
                ac_coeffs.push_back(0);
            }
            return ac_coeffs;
        }
        if (code.run == 15 && code.value == 0) {
            for (int i = 0; i < 16; i++) {
                ac_coeffs.push_back(0);
            }
            continue;
        }
        for (int i = 0; i < code.run; i++) {
            ac_coeffs.push_back(0);
        }
        ac_coeffs.push_back(code.value);
    }
    return ac_coeffs;
}

Jpeg::Huffman_table::Huffman_table(const vector<uint8_t>& bits, const vector<uint8_t>& values) {
    codes.resize(256);
    lens.resize(256);

    uint16_t code = 0;
    size_t k = 0;

    for (int l = 1; l <= 16; l++) {
        for (int i = 0; i < bits[l]; i++) {
            uint8_t symbol = values[k++];
            codes[symbol] = code;
            lens[symbol] = (uint8_t)l;
            code++;
        }
        code <<= 1;
    }
    for (int s = 0; s < 256; s++) {
        if (lens[s] != 0) {
            reversed[codes[s]].first = s;
            reversed[codes[s]].second = lens[s];
        }
    }
}

uint8_t getCategory(int value) {
    if (value == 0) return 0;
    uint32_t v = abs(value);
    uint8_t category = 0;
    while (v > 0) { v >>= 1; category++; }
    return category;
}

void Jpeg::VLC::DC_huffman_encode(const int& diff, vector<bool>& output, bool isLuminance) {
    uint8_t category = getCategory(diff);
    uint16_t raw_bits;

    if (diff > 0) raw_bits = static_cast<uint16_t>(diff);
    else raw_bits = static_cast<uint16_t>((1 << category) - 1 + diff);

    uint16_t bits = isLuminance ? DC_luminance.codes[category] : DC_chrominance.codes[category];
    uint8_t length = isLuminance ? DC_luminance.lens[category] : DC_chrominance.lens[category];

    for (int i = length - 1; i >= 0; i--) {
        output.push_back((bits >> i) & 1);
    }
    for (int i = category - 1; i >= 0; i--) {
        output.push_back((raw_bits >> i) & 1);
    }
}
int Jpeg::VLC::DC_huffman_decode(const vector<bool>& data, uint64_t& i, bool isLuminance) {
    uint16_t code = 0;
    uint8_t code_len = 0;
    uint64_t start = i;

    auto& reversed = isLuminance ? DC_luminance.reversed : DC_chrominance.reversed;

    while (true) {
        code = (code << 1) | (data[i++] ? 1 : 0);
        code_len++;

        auto it = reversed.find(code);
        if (it == reversed.end()) {
            continue;
        }
        uint8_t category = it->second.first;
        uint8_t length = it->second.second;

        if (i - start != length) {
            continue;
        }
        uint16_t raw_bits = 0;
        for (int j = 0; j < category; j++) {
            raw_bits = (raw_bits << 1) | (data[i++] ? 1 : 0);
        }
        int diff;
        if (raw_bits >= (1 << (category - 1))) diff = static_cast<int>(raw_bits);
        else diff = static_cast<int>(raw_bits - (1 << category) + 1);
        return diff;
    }
}

void Jpeg::VLC::AC_huffman_encode(const vector<Jpeg::RLE_code>& data, vector<bool>& output, bool isLuminance) {
    auto& codes = isLuminance ? AC_luminance.codes : AC_chrominance.codes;
    auto& lengths = isLuminance ? AC_luminance.lens : AC_chrominance.lens;

    for (const auto& value : data) {
        uint8_t size = getCategory(value.value);
        uint8_t runsize = (value.run << 4) | size;
        uint16_t code = codes[runsize];
        uint8_t code_length = lengths[runsize];

        for (int j = code_length - 1; j >= 0; j--) {
            output.push_back((code >> j) & 1);
        }
        if (size > 0) {
            uint16_t raw_bits;
            if (value.value > 0) {
                raw_bits = static_cast<uint16_t>(value.value);
            }
            else {
                raw_bits = static_cast<uint16_t>((1 << size) - 1 + value.value);
            }
            for (int j = size - 1; j >= 0; j--) {
                output.push_back((raw_bits >> j) & 1);
            }
        }
    }
}

vector <Jpeg::RLE_code> Jpeg::VLC::AC_huffman_decode(const vector<bool>& data, uint64_t& i, bool isLuminance) {
    vector<RLE_code> output;
    uint16_t count_decode_values = 0;
    auto& reversed = isLuminance ? AC_luminance.reversed : AC_chrominance.reversed;

    while (count_decode_values < 63) {
        uint16_t code = 0;
        uint64_t start = i;

        while (true) {
            code = (code << 1) | (data[i++] ? 1 : 0);

            auto it = reversed.find(code);
            if (it == reversed.end()) {
                continue;
            }
            uint8_t runsize = it->second.first;
            uint8_t length = it->second.second;

            if (i - start != length) {
                continue;
            }
            if (runsize == 0x00) {
                output.emplace_back(0, 0);
                return output;
            }
            if (runsize == 0xF0) {
                output.emplace_back(16, 0);
                count_decode_values += 16;
                break;
            }

            uint8_t run = (runsize >> 4) & 0x0F;
            uint8_t size = runsize & 0x0F;
            uint16_t raw_bits = 0;

            for (int j = 0; j < size; j++) {
                raw_bits = (raw_bits << 1) | (data[i++] ? 1 : 0);
            }
            int16_t value;
            if (raw_bits & (1 << (size - 1))) {
                value = static_cast<int16_t>(raw_bits);
            }
            else {
                value = static_cast<int16_t>(raw_bits - (1 << size) + 1);
            }

            output.emplace_back(run, value);
            count_decode_values += run + 1;
            break;
        }
    }
    return output;
}

vector<bool> Jpeg::VLC::encode(const vector<Block>& y_blocks,
    const vector<Block>& cb_blocks,
    const vector<Block>& cr_blocks,
    uint16_t img_width, uint16_t img_height,
    uint8_t ds_coeff) {
    vector<bool> output;

    vector<int> dc_coeffs_Y, dc_coeffs_Cb, dc_coeffs_Cr;
    for (const auto& b : y_blocks) dc_coeffs_Y.push_back(b.data[0]);
    for (const auto& b : cb_blocks) dc_coeffs_Cb.push_back(b.data[0]);
    for (const auto& b : cr_blocks) dc_coeffs_Cr.push_back(b.data[0]);

    dc_coeffs_Y = DC_difference_encode(dc_coeffs_Y);
    dc_coeffs_Cb = DC_difference_encode(dc_coeffs_Cb);
    dc_coeffs_Cr = DC_difference_encode(dc_coeffs_Cr);

    bool is_rgb = false;
    for (int v : dc_coeffs_Cb) if (v != 0) { is_rgb = true; break; }
    if (!is_rgb) for (int v : dc_coeffs_Cr) if (v != 0) { is_rgb = true; break; }
    output.push_back(is_rgb);

    uint16_t blocks_per_row = ceil(float(img_width) / 8 / float(ds_coeff));
    uint16_t blocks_per_col = ceil(float(img_height) / 8 / float(ds_coeff));
    uint16_t y_blocks_per_row = blocks_per_row * ds_coeff;

    for (uint16_t y = 0; y < blocks_per_col; y++) {
        for (uint16_t x = 0; x < blocks_per_row; x++) {
            for (uint8_t dy = 0; dy < ds_coeff; dy++) {
                for (uint8_t dx = 0; dx < ds_coeff; dx++) {
                    uint16_t y_idx = (y * ds_coeff + dy) * y_blocks_per_row + (x * ds_coeff + dx);
                    const auto& block = y_blocks[y_idx];

                    vector<RLE_code> ac_codes = AC_RLE_encode(block.data.begin() + 1, block.data.end());
                    DC_huffman_encode(dc_coeffs_Y[y_idx], output, true);
                    AC_huffman_encode(ac_codes, output, true);
                }
            }

            if (is_rgb) {
                uint16_t chroma_idx = y * blocks_per_row + x;

                auto& cb_block = cb_blocks[chroma_idx];
                vector<RLE_code> ac_codes_cb = AC_RLE_encode(cb_block.data.begin() + 1, cb_block.data.end());
                DC_huffman_encode(dc_coeffs_Cb[chroma_idx], output, false);
                AC_huffman_encode(ac_codes_cb, output, false);

                auto& cr_block = cr_blocks[chroma_idx];
                vector<RLE_code> ac_codes_cr = AC_RLE_encode(cr_block.data.begin() + 1, cr_block.data.end());
                DC_huffman_encode(dc_coeffs_Cr[chroma_idx], output, false);
                AC_huffman_encode(ac_codes_cr, output, false);
            }
        }
    }

    return output;
}

void Jpeg::VLC::decode(const vector<bool>& data,
    vector<Block>& y_blocks,
    vector<Block>& cb_blocks,
    vector<Block>& cr_blocks,
    uint16_t img_width, uint16_t img_height,
    uint8_t ds_coeff) {
    size_t i = 0;
    bool is_rgb = data[i++];

    uint16_t blocks_per_row = ceil(float(img_width) / 8 / float(ds_coeff));
    uint16_t blocks_per_col = ceil(float(img_height) / 8 / float(ds_coeff));
    uint16_t y_blocks_per_row = blocks_per_row * ds_coeff;
    uint16_t y_total_blocks = y_blocks_per_row * blocks_per_col * ds_coeff;
    uint16_t chroma_total_blocks = blocks_per_row * blocks_per_col;

    y_blocks.resize(y_total_blocks, Block(8, 8));
    cb_blocks.resize(chroma_total_blocks, Block(8, 8));
    cr_blocks.resize(chroma_total_blocks, Block(8, 8));

    vector<int> dc_coeffs_Y(y_total_blocks, 0);
    vector<int> dc_coeffs_Cb(chroma_total_blocks, 0);
    vector<int> dc_coeffs_Cr(chroma_total_blocks, 0);

    for (uint16_t y = 0; y < blocks_per_col; y++) {
        for (uint16_t x = 0; x < blocks_per_row; x++) {
            for (uint8_t dy = 0; dy < ds_coeff; dy++) {
                for (uint8_t dx = 0; dx < ds_coeff; dx++) {
                    uint16_t y_idx = (y * ds_coeff + dy) * y_blocks_per_row + (x * ds_coeff + dx);

                    dc_coeffs_Y[y_idx] = DC_huffman_decode(data, i, true);
                    vector<RLE_code> ac_codes = AC_huffman_decode(data, i, true);
                    y_blocks[y_idx].data = AC_RLE_decode(ac_codes);
                }
            }

            if (is_rgb) {
                uint16_t chroma_idx = y * blocks_per_row + x;

                dc_coeffs_Cb[chroma_idx] = DC_huffman_decode(data, i, false);
                vector<RLE_code> ac_codes_cb = AC_huffman_decode(data, i, false);
                cb_blocks[chroma_idx].data = AC_RLE_decode(ac_codes_cb);

                dc_coeffs_Cr[chroma_idx] = DC_huffman_decode(data, i, false);
                vector<RLE_code> ac_codes_cr = AC_huffman_decode(data, i, false);
                cr_blocks[chroma_idx].data = AC_RLE_decode(ac_codes_cr);
            }
            else {
                uint16_t chroma_idx = y * blocks_per_row + x;
                cb_blocks[chroma_idx].data.resize(63, 0);
                cr_blocks[chroma_idx].data.resize(63, 0);
            }
        }
    }

    dc_coeffs_Y = DC_difference_decode(dc_coeffs_Y);
    if (is_rgb) {
        dc_coeffs_Cb = DC_difference_decode(dc_coeffs_Cb);
        dc_coeffs_Cr = DC_difference_decode(dc_coeffs_Cr);
    }

    for (size_t idx = 0; idx < y_blocks.size(); idx++) {
        y_blocks[idx].data.insert(y_blocks[idx].data.begin(), dc_coeffs_Y[idx]);
    }
    for (size_t idx = 0; idx < cb_blocks.size(); idx++) {
        cb_blocks[idx].data.insert(cb_blocks[idx].data.begin(), dc_coeffs_Cb[idx]);
        cr_blocks[idx].data.insert(cr_blocks[idx].data.begin(), dc_coeffs_Cr[idx]);
    }
}

vector<uint8_t> Jpeg::combine_metadata(
    uint16_t width, uint16_t height,
    uint8_t ds_coeff,
    const Quantisator quantisator,
    const vector<uint8_t>& dc_luminance_bits, const vector<uint8_t>& dc_luminance_values,
    const vector<uint8_t>& dc_chrominance_bits, const vector<uint8_t>& dc_chrominance_values,
    const vector<uint8_t>& ac_luminance_bits, const vector<uint8_t>& ac_luminance_values,
    const vector<uint8_t>& ac_chrominance_bits, const vector<uint8_t>& ac_chrominance_values,
    const vector<bool>& data
) {
    vector<uint8_t> result;

    result.push_back((width >> 8) & 0xFF);
    result.push_back(width & 0xFF);
    result.push_back((height >> 8) & 0xFF);
    result.push_back(height & 0xFF);

    result.push_back(ds_coeff);

    result.insert(result.end(), quantisator.luminance.begin(), quantisator.luminance.end());
    result.insert(result.end(), quantisator.chrominance.begin(), quantisator.chrominance.end());

    result.insert(result.end(), dc_luminance_bits.begin(), dc_luminance_bits.end());
    result.insert(result.end(), dc_luminance_values.begin(), dc_luminance_values.end());
    result.insert(result.end(), dc_chrominance_bits.begin(), dc_chrominance_bits.end());
    result.insert(result.end(), dc_chrominance_values.begin(), dc_chrominance_values.end());
    result.insert(result.end(), ac_luminance_bits.begin(), ac_luminance_bits.end());
    result.insert(result.end(), ac_luminance_values.begin(), ac_luminance_values.end());
    result.insert(result.end(), ac_chrominance_bits.begin(), ac_chrominance_bits.end());
    result.insert(result.end(), ac_chrominance_values.begin(), ac_chrominance_values.end());

    uint8_t byte = 0;
    int bit_index = 7;
    for (bool b : data) {
        byte |= b << bit_index;
        bit_index--;
        if (bit_index < 0) {
            result.push_back(byte);
            byte = 0;
            bit_index = 7;
        }
    }
    result.push_back(byte);
    result.push_back(7 - bit_index);
    return result;
}

void Jpeg::restore_metadata(
    const vector<uint8_t>& data,
    uint16_t& width, uint16_t& height,
    uint8_t& ds_coeff,
    Quantisator*& quantisator,
    VLC*& vlc,
    vector<bool>& raw_bits
) {
    uint64_t i = 0;
    width = (data[i] << 8) | (data[i + 1]);
    i += 2;
    height = (data[i] << 8) | (data[i + 1]);
    i += 2;

    ds_coeff = data[i++];

    vector<uint8_t> luminance_quant_table{ data.begin() + i, data.begin() + i + 64 };
    i += 64;
    vector<uint8_t> chrominance_quant_table{ data.begin() + i, data.begin() + i + 64 };
    i += 64;
    quantisator = new Quantisator(luminance_quant_table, chrominance_quant_table);

    vector<uint8_t> dc_l_bits{ data.begin() + i, data.begin() + i + 17 };
    i += 17;
    vector<uint8_t> dc_l_values{ data.begin() + i, data.begin() + i + 12 };
    i += 12;
    vector<uint8_t> dc_c_bits{ data.begin() + i, data.begin() + i + 17 };
    i += 17;
    vector<uint8_t> dc_c_values{ data.begin() + i, data.begin() + i + 12 };
    i += 12;
    vector<uint8_t> ac_l_bits{ data.begin() + i, data.begin() + i + 17 };
    i += 17;
    vector<uint8_t> ac_l_values{ data.begin() + i, data.begin() + i + 162 };
    i += 162;
    vector<uint8_t> ac_c_bits{ data.begin() + i, data.begin() + i + 17 };
    i += 17;
    vector<uint8_t> ac_c_values{ data.begin() + i, data.begin() + i + 162 };
    i += 162;
    vlc = new VLC(dc_l_bits, dc_l_values, dc_c_bits, dc_c_values, ac_l_bits, ac_l_values, ac_c_bits, ac_c_values);

    uint8_t last_bits = data[data.size() - 1];
    for (; i < data.size() - 2; i++) {
        uint8_t byte = data[i];
        for (int bit_index = 7; bit_index >= 0; bit_index--) {
            raw_bits.push_back((byte >> bit_index) & 1);
        }
    }
    for (int bit_index = 7; bit_index > 7 - last_bits; bit_index--) {
        raw_bits.push_back((data[i] >> bit_index) & 1);
    }
}