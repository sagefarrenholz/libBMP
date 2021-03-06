/* Copyright 2020, Sage Farrenholz */

#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <exception>
#include <stdexcept>

#include "BMP_header.h"

using color = uint32_t;

class BMP {
    /*
     -
     - I found the following links useful for information about the BMP image format and bit depth.
     -
     == BMP File Format:
     * Wikipedia link --> https://en.wikipedia.org/wiki/BMP_file_format#DIB_header_(bitmap_information_header)
     * Archived link --> https://web.archive.org/web/20200508042056/https://en.wikipedia.org/wiki/BMP_file_format
     - 
     == Color Bit Depth:
     * Wikipedia link --> https://en.wikipedia.org/wiki/Color_depth  
     * Archived link --> https://web.archive.org/web/20200623202619/https://en.wikipedia.org/wiki/Color_depth
     - 
     */

    //---------------- Image Data ---------------
    
    // Header required for generation, contains size and header.
    BMP_Header header; 

    // Vector containing all of the image's pixels.
    uint8_t* data = NULL;

    // 32bit palette, required for images <= 8 bit depth.
    uint32_t* palette = NULL;

    //--------------- Image attribute operations ---------------

    public:

    // Set the bit depth of the image.
    void set_bit_depth(const uint16_t& bit_depth) {
        switch (bit_depth) {
            case 1:
            case 2:
            case 4:
            case 8:
		if (palette != nullptr) free(palette);
                palette = reinterpret_cast<uint32_t*>(calloc(sizeof(uint32_t), pow_2(bit_depth)));
                header.bit_depth = bit_depth;
                break;
            case 16:
            case 24:
            case 32:
		if (palette != nullptr) {
			free(palette);
			palette = nullptr;
		}
                header.bit_depth = bit_depth;
                break;
            default:
                throw std::logic_error("Attempting to set invalid bitdepth.");
        }
    }

    // Set the size of the image.
    void set_size(const int32_t& width, const int32_t& height) {
        header.width = width;
        header.height = height;
        // Calculates image data width including padding.
        calc_raw_width();
        free(data);
        data = reinterpret_cast<uint8_t*>(calloc(sizeof(uint8_t), raw_width * height));
    }

    // Sets the resolution of the image, in pixels per meter.
    void set_resolution(int32_t x_res, int32_t y_res) {
        header.x_res = x_res;
        header.y_res = y_res;
    }

    //---------- Pixel operations -----------
    
    // By coordinate, indexed at (0, 0).
    void set_pixel(const int32_t& x, const int32_t& y, const color&);

    // By index, indexed at 0.
    void set_pixel(const size_t& index, const color& );

    // Sets a color in the palette.
    void set_palette(const int& i, const color&); 

    // Fill the image with one color.
    void fill(const color&);

    // Fill one row with a color.
    void fill_row(const int32_t& row, const color&);

    // Fill one col with a color.
    void fill_col(const int32_t& col, const color&);

    // Fill rectangle with a color.
    void fill_rect(const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, const color&); 

    //---------- Information ----------

    uint16_t get_bit_depth() const { return header.bit_depth; }

    int32_t get_width() const { return header.width; }

    int32_t get_height() const { return header.height; }

    int64_t get_size() const { return header.width * header.height; }
    
    // Returns the x resolution of the image in ppm
    int32_t get_x_res() const { return header.x_res; }

    // Returns the y resolution of the image in ppm
    int32_t get_y_res() const { return header.y_res; }

    // Returns a pixel by index, indexed at 0.
    color get_pixel(const size_t& index) const;

    // Returns a pixel by coordinate, index begins at given argument.
    color get_pixel(const int32_t& x, const int32_t& y) const; 
    
    // Returns a palette color.
    color get_palette(const uint8_t& index) const;

    // Returns the number of available palette colors.
    uint16_t get_palette_size() const;

    //--------------- I/O ---------------

    // Copy from a BMP image file.
    void copy(const std::string& file);
    
    // Copy from another BMP object
    void copy(const BMP& from);

    // Generate the current image to the output directory.
    void generate(const std::string& name);
    
    //--------------- Constructors ----------------
    
    explicit BMP(int32_t width = 64, int32_t height = 64, uint16_t bitdepth = 24) {
        set_bit_depth(bitdepth);    
        set_size(width, height);
    };

    explicit BMP(const std::string& filename) {
        copy(filename);
    };  

    explicit BMP(const BMP& from) {
        copy(from);
    }

    //--------------- Destructor ---------------
    ~BMP() {
        free(data);
        free(palette);
    }
    
    //--------------- Internal Tools ---------------

    private:
    
    static constexpr uint64_t pow_2(const unsigned& n) { return 1 << n; }
    uint16_t pal_size() const { return 4 * pow_2(get_bit_depth()); }
    
    // Actual size in bytes of one row.
    unsigned raw_width = 0;

    // Calculates the raw_width variable.
    void calc_raw_width();

    // Returns the width of the image in bytes, including padding.
    unsigned get_raw_width() const { return raw_width; }

    // Returns a pointer to a row in the raw data.
    uint8_t* get_row(const uint32_t& i) const;

    // Returns a pointer to byte containing the given pixel.
    uint8_t* get_pixel_ptr(const size_t& i) const;
};
