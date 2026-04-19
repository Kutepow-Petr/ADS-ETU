#include <iostream>
#include <regex>
#include "File_Reader.h"
#include "Jpeg.h"

int main(int argc, char* argv[]) {
    string img_path;
    cout << "Enter image path: ";
    cin >> img_path;

    img_path.erase(remove(img_path.begin(), img_path.end(), '\"'), img_path.end());
    int dot_pos = img_path.find_last_of('.');
    int slash_pos = img_path.find_last_of('\\') + 1;
    string img_name = img_path.substr(slash_pos, dot_pos - slash_pos);
    regex pattern(R"((\d+)x(\d+))");
    smatch matches;
    regex_search(img_path, matches, pattern);
    uint16_t width = stoi(matches[1].str()), height = stoi(matches[2].str());

    File_Reader file_reader;
    vector<Jpeg::RGB> raw_pixels = file_reader.read_file<Jpeg::RGB>(img_path);

    Jpeg jpeg;
    vector<Jpeg::YCbCr> ycbcr_format = jpeg.rgb_to_ycbcr(raw_pixels);

    //ENCODE
    vector<uint8_t> y;
    vector<uint8_t> cb;
    vector<uint8_t> cr;
    uint8_t ds_coeff = 2;
    jpeg.downsample(ycbcr_format, y, cb, cr, width, height, ds_coeff);
    vector<Jpeg::Block> y_blocks = jpeg.split_into_blocks(y, width, height, 8, 8);
    vector<Jpeg::Block> cb_blocks = jpeg.split_into_blocks(cb, width / ds_coeff, height / ds_coeff, 8, 8);
    vector<Jpeg::Block> cr_blocks = jpeg.split_into_blocks(cr, width / ds_coeff, height / ds_coeff, 8, 8);
    Jpeg::DCT* dct = new Jpeg::DCT();
    dct->forward_blocks(y_blocks, true);
    dct->forward_blocks(cb_blocks, true);
    dct->forward_blocks(cr_blocks, true);
    Jpeg::Quantisator* quantisator = new Jpeg::Quantisator(50);
    quantisator->quant_coefficient(y_blocks, true);
    quantisator->quant_coefficient(cb_blocks, false);
    quantisator->quant_coefficient(cr_blocks, false);
    for (auto& block : y_blocks) { block.zigzag_forward(); }
    for (auto& block : cb_blocks) { block.zigzag_forward(); }
    for (auto& block : cr_blocks) { block.zigzag_forward(); }
    Jpeg::VLC* vlc = new Jpeg::VLC();
    vector<bool> raw_bits = vlc->encode(y_blocks, cb_blocks, cr_blocks, width, height, ds_coeff);
    vector<uint8_t> encoded_data = jpeg.combine_metadata(
        width, height,
        ds_coeff,
        *quantisator,
        DC_LUMINANCE_BITS, DC_LUMINANCE_VALUES,
        DC_CHROMINANCE_BITS, DC_CHROMINANCE_VALUES,
        AC_LUMINANCE_BITS, AC_LUMINANCE_VALUES,
        AC_CHROMINANCE_BITS, AC_CHROMINANCE_VALUES,
        raw_bits
    );
    file_reader.vector_to_file<uint8_t>(encoded_data, "./result/" + img_name + "_encode.raw");



    //DECODE
    uint16_t decoded_width, decoded_height;
    uint8_t decoded_ds_coeff;
    Jpeg::Quantisator* decoded_quantisator;
    Jpeg::VLC* decoded_vlc;
    vector<bool> decoded_raw_bits;
    vector<uint8_t> decoded_data = file_reader.read_file<uint8_t>("./result/" + img_name + "_encode.raw");
    jpeg.restore_metadata(
        decoded_data,
        decoded_width, decoded_height,
        decoded_ds_coeff,
        decoded_quantisator,
        decoded_vlc,
        decoded_raw_bits
    );
    vector<Jpeg::Block> decoded_y_blocks;
    vector<Jpeg::Block> decoded_cb_blocks;
    vector<Jpeg::Block> decoded_cr_blocks;
    decoded_vlc->decode(
        decoded_raw_bits, decoded_y_blocks, decoded_cb_blocks, decoded_cr_blocks,
        decoded_width, decoded_height, decoded_ds_coeff
    );
    for (auto& block : decoded_y_blocks) { block.zigzag_inverse(); }
    for (auto& block : decoded_cb_blocks) { block.zigzag_inverse(); }
    for (auto& block : decoded_cr_blocks) { block.zigzag_inverse(); }
    decoded_quantisator->norm_coefficient(decoded_y_blocks, true);
    decoded_quantisator->norm_coefficient(decoded_cb_blocks, false);
    decoded_quantisator->norm_coefficient(decoded_cr_blocks, false);
    dct->inverse_blocks(decoded_y_blocks, true);
    dct->inverse_blocks(decoded_cb_blocks, true);
    dct->inverse_blocks(decoded_cr_blocks, true);
    vector<uint8_t> decoded_y = jpeg.blocks_to_vector(decoded_y_blocks, decoded_width, decoded_height);
    vector<uint8_t> decoded_cb = jpeg.blocks_to_vector(decoded_cb_blocks, decoded_width / decoded_ds_coeff, decoded_height / decoded_ds_coeff);
    vector<uint8_t> decoded_cr = jpeg.blocks_to_vector(decoded_cr_blocks, decoded_width / decoded_ds_coeff, decoded_height / decoded_ds_coeff);
    vector<Jpeg::YCbCr> decoded_upsampled = jpeg.upsample(decoded_y, decoded_cb, decoded_cr, decoded_width, decoded_height, decoded_ds_coeff);
    vector<Jpeg::RGB> rgb_format = jpeg.ycbcr_to_rgb(decoded_upsampled);
    file_reader.vector_to_file<Jpeg::RGB>(rgb_format, "./result/" + img_name + "_decode.raw");
}