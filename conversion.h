#ifndef CONVERSION_H
#define CONVERSION_H
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>




class conversion
{
private:
    void convert_rgb_to_bgr(std::vector<unsigned char> &pixels, int width, int height);
    bool write_bmp(const std::string &filename, int width, int height, const std::vector<unsigned char> &pixels);
    bool read_jpeg(const std::string &filename, int &width, int &height, std::vector<unsigned char> &pixels);
    bool bmp_to_jpeg(const std::string &bmp_filename, const std::string &jpeg_filename, int quality);
    bool read_bmp(const std::string &filename, int &width, int &height, std::vector<unsigned char> &pixels);

    bool jpeg_to_png(const std::string& jpeg_path, const std::string& png_path);
    bool png_to_jpeg(const std::string& png_path, const std::string& jpeg_path, int quality);
    bool png_to_bmp(const std::string& png_path, const std::string& bmp_path);
    bool bmp_to_png(const std::string& bmp_path, const std::string& png_path);

public:
    conversion(/* args */);
    ~conversion();

    void ConvertJPGtoBMP(std::string Input, std::string Output);
    void ConvertBMPtoJPG(std::string Input, std::string Output);
    void ConvertPNGtoJPG(std::string Input, std::string Output);
    void ConvertJPGtoPNG(std::string Input, std::string Output);
    void ConvertPNGtoBMP(std::string Input, std::string Output);
    void ConvertBMPtoPNG(std::string Input, std::string Output);

    
};


