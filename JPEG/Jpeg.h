#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <math.h>
#include "Сoefficients.h"
using namespace std;

class Jpeg {
public:
    struct RGB {
        uint8_t r, g, b;
    };
    struct YCbCr {
        uint8_t y, cb, cr;
    };
    vector<YCbCr> rgb_to_ycbcr(const vector<RGB>& data);
    vector<RGB> ycbcr_to_rgb(const vector<YCbCr>& data);

    void downsample(const vector<YCbCr>& data, vector<uint8_t>& res_y, vector<uint8_t>& res_cb, vector<uint8_t>& res_cr, uint16_t& width, uint16_t& height, uint8_t coeff);
    vector<YCbCr> upsample(const vector<uint8_t>& data_y, const vector<uint8_t>& data_cb, const vector<uint8_t>& data_cr, uint16_t width, uint16_t height, uint8_t coeff);
    vector<YCbCr> bilinear_upsample(const vector<uint8_t>& data_y, const vector<uint8_t>& data_cb, const vector<uint8_t>& data_cr, uint16_t width, uint16_t height, uint8_t coeff);
    uint8_t linear_interpolation(uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2, uint16_t x);
    uint8_t linear_spline(vector<uint16_t> X, vector<uint8_t> Y, uint16_t x);
    uint8_t bilinear_interpolation(vector<pair<uint16_t, uint16_t>> XY, vector<uint8_t> Z, uint16_t x, uint16_t y);

    struct Block {
        uint8_t w;
        uint8_t h;
        vector<int> data;
        Block(uint8_t w, uint8_t h) : w(w), h(h) {};
        
        void zigzag_forward();
        void zigzag_inverse();
    };
    vector<Block> split_into_blocks(const vector<uint8_t>& data, uint16_t width, uint16_t height, uint8_t block_width, uint8_t block_height);
    vector<uint8_t> blocks_to_vector(const vector<Block>& data, uint16_t width, uint16_t height);

    class DCT {
    private:
        void forward_basic(vector<int>& data, uint8_t m, uint8_t n);
        void inverse_basic(vector<int>& data, uint8_t m, uint8_t n);
        vector<float> multiply(const vector<float>& A, const vector<float>& B);
        void forward_fast(vector<int>& data);
        void inverse_fast(vector<int>& data);
    public:
        void forward_blocks(vector<Block>& data, bool fast);
        void inverse_blocks(vector<Block>& data, bool fast);
    };

    class Quantisator {
    public:
        vector<uint8_t> luminance;
        vector<uint8_t> chrominance;
        Quantisator(uint8_t quality);
        Quantisator(vector<uint8_t>& l, vector<uint8_t>& c) : luminance(l), chrominance(c) {};
        void quant_coefficient(vector<Block>& data, bool isLuminance);
        void norm_coefficient(vector<Block>& data, bool isLuminance);
    };

    struct RLE_code {
        uint8_t run;
        int value;
        RLE_code(uint8_t r, int v) : run(r), value(v) {}
    };
    struct Huffman_table {
        vector<uint16_t> codes;
        vector<uint8_t> lens;
        unordered_map<uint16_t, pair<uint8_t, uint8_t>> reversed;
        Huffman_table(const vector<uint8_t>& bits, const vector<uint8_t>& values);
    };
    class VLC {
    private:
        vector<int> DC_difference_encode(const vector<int>& data);
        vector<int> DC_difference_decode(const vector<int>& data);

        template<typename Iterator>
        vector<RLE_code> AC_RLE_encode(Iterator begin, Iterator end);
        vector<int> AC_RLE_decode(const vector<RLE_code>& data);

        void DC_huffman_encode(const int& diff, vector<bool>& output, bool isLuminance);
        int DC_huffman_decode(const vector<bool>& data, uint64_t& i, bool isLuminance);

        void AC_huffman_encode(const vector<Jpeg::RLE_code>& data, vector<bool>& output, bool isLuminance);
        vector<RLE_code> AC_huffman_decode(const vector<bool>& data, uint64_t& i, bool isLuminance);
    public:
        Huffman_table DC_luminance;
        Huffman_table DC_chrominance;
        Huffman_table AC_luminance;
        Huffman_table AC_chrominance;
        VLC() : DC_luminance(DC_LUMINANCE_BITS, DC_LUMINANCE_VALUES),
            DC_chrominance(DC_CHROMINANCE_BITS, DC_CHROMINANCE_VALUES),
            AC_luminance(AC_LUMINANCE_BITS,AC_LUMINANCE_VALUES),
            AC_chrominance(AC_CHROMINANCE_BITS, AC_CHROMINANCE_VALUES) {};
        VLC(
            vector<uint8_t>& dc_l_bits, vector<uint8_t>& dc_l_vals,
            vector<uint8_t>& dc_c_bits, vector<uint8_t>& dc_c_vals,
            vector<uint8_t>& ac_l_bits, vector<uint8_t>& ac_l_vals,
            vector<uint8_t>& ac_c_bits, vector<uint8_t>& ac_c_vals
        ) : DC_luminance(dc_l_bits, dc_l_vals),
            DC_chrominance(dc_c_bits, dc_c_vals),
            AC_luminance(ac_l_bits, ac_l_vals),
            AC_chrominance(ac_c_bits, ac_c_vals) {};

        vector<bool> encode(const vector<Block>& y_blocks, const vector<Block>& cb_blocks, const vector<Block>& cr_blocks, uint16_t img_width, uint16_t img_height, uint8_t ds_coeff);
        void decode(const vector<bool>& data, vector<Block>& y_blocks, vector<Block>& cb_blocks, vector<Block>& cr_blocks, uint16_t img_width, uint16_t img_height, uint8_t ds_coeff);
    };

    vector<uint8_t> combine_metadata(
        uint16_t width, uint16_t height,
        uint8_t ds_coeff,
        const Quantisator quantisator,
        const vector<uint8_t>& dc_luminance_bits, const vector<uint8_t>& dc_luminance_values,
        const vector<uint8_t>& dc_chrominance_bits, const vector<uint8_t>& dc_chrominance_values,
        const vector<uint8_t>& ac_luminance_bits, const vector<uint8_t>& ac_luminance_values,
        const vector<uint8_t>& ac_chrominance_bits, const vector<uint8_t>& ac_chrominance_values,
        const vector<bool>& data
    );
    void restore_metadata(
        const vector<uint8_t>& data,
        uint16_t& width, uint16_t& height,
        uint8_t& ds_coeff,
        Quantisator*& quantisator,
        VLC*& vlc,
        vector<bool>& raw_bits
    );
};