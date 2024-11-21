#include "conversion.h"
#include <stdio.h>
#include <cstdio> 
#include <png.h>
#include <jpeglib.h>

conversion::conversion()
{
}

conversion::~conversion()
{
}

bool conversion::read_bmp(const std::string &filename, int &width, int &height, std::vector<unsigned char> &pixels) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening BMP file" << std::endl;
        return false;
    }

    // Lire l'en-tête BMP
    unsigned char header[54];
    file.read(reinterpret_cast<char *>(header), 54);

    if (header[0] != 'B' || header[1] != 'M') {
        std::cerr << "Not a valid BMP file" << std::endl;
        return false;
    }

    // Extraire les dimensions de l'image
    width = *reinterpret_cast<int *>(&header[18]);
    height = *reinterpret_cast<int *>(&header[22]);

    int row_size = (width * 3 + 3) & ~3; // Aligner les lignes sur 4 octets
    int image_size = row_size * height;

    // Lire les pixels
    pixels.resize(image_size);
    file.seekg(54, std::ios::beg); // Sauter l'en-tête BMP
    file.read(reinterpret_cast<char *>(pixels.data()), image_size);
    file.close();

    // Convertir BGR → RGB
    for (size_t i = 0; i < pixels.size(); i += 3) {
        std::swap(pixels[i], pixels[i + 2]);
    }

    // Inverser verticalement les pixels
    std::vector<unsigned char> flipped_pixels(pixels.size());
    for (int y = 0; y < height; ++y) {
        std::copy(
            pixels.begin() + y * row_size,
            pixels.begin() + (y + 1) * row_size,
            flipped_pixels.begin() + (height - 1 - y) * row_size
        );
    }
    pixels.swap(flipped_pixels);

    return true;
}

bool conversion::bmp_to_jpeg(const std::string &bmp_filename, const std::string &jpeg_filename, int quality) {
    int width, height;
    std::vector<unsigned char> pixels;

    // Lire l'image BMP
    if (!read_bmp(bmp_filename, width, height, pixels)) {
        return false;
    }

    // Créer un objet de compression JPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Ouvrir le fichier de sortie JPEG
    FILE *outfile = fopen(jpeg_filename.c_str(), "wb");
    if (!outfile) {
        std::cerr << "Error opening output JPEG file" << std::endl;
        return false;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    // Définir les paramètres de compression JPEG
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;  // RGB
    cinfo.in_color_space = JCS_RGB;  // Utilisation du format couleur RGB

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);  // Définir la qualité du JPEG (0 à 100)

    // Démarrer la compression
    jpeg_start_compress(&cinfo, TRUE);

    // Écrire les lignes de l'image
    int row_stride = width * 3;  // 3 pour RGB
    unsigned char *row_pointer = nullptr;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = &pixels[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    // Fin de la compression
    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    // Libérer la mémoire utilisée par libjpeg
    jpeg_destroy_compress(&cinfo);

    std::cout << "JPEG file created: " << jpeg_filename << std::endl;
    return true;
}

bool conversion::read_jpeg(const std::string &filename, int &width, int &height, std::vector<unsigned char> &pixels) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE *infile = fopen(filename.c_str(), "rb");
    if (!infile) {
        std::cerr << "Erreur d'ouverture du fichier JPEG : " << filename << std::endl;
        return false;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    int row_stride = width * cinfo.output_components;

    pixels.resize(row_stride * height);
    unsigned char *row_pointer = nullptr;

    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer = &pixels[cinfo.output_scanline * row_stride];
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return true;
}

bool conversion::write_bmp(const std::string &filename, int width, int height, const std::vector<unsigned char> &pixels) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erreur d'ouverture du fichier BMP : " << filename << std::endl;
        return false;
    }

    // Taille de l'image (avec padding éventuel)
    int row_size = (width * 3 + 3) & ~3; // Alignement à 4 octets
    int image_size = row_size * height;

    // En-tête BMP (54 octets)
    unsigned char header[54] = {0};
    header[0] = 'B';
    header[1] = 'M';
    *reinterpret_cast<int *>(&header[2]) = 54 + image_size; // Taille totale du fichier
    *reinterpret_cast<int *>(&header[10]) = 54;            // Offset des données d'image
    *reinterpret_cast<int *>(&header[14]) = 40;            // Taille de l'en-tête DIB
    *reinterpret_cast<int *>(&header[18]) = width;         // Largeur
    *reinterpret_cast<int *>(&header[22]) = height;        // Hauteur
    *reinterpret_cast<short *>(&header[26]) = 1;           // Nombre de plans
    *reinterpret_cast<short *>(&header[28]) = 24;          // Bits par pixel
    *reinterpret_cast<int *>(&header[34]) = image_size;    // Taille des données d'image

    // Écrire l'en-tête BMP
    file.write(reinterpret_cast<char *>(header), 54);

    // Écrire les pixels (en inversant les lignes)
    std::vector<unsigned char> padded_row(row_size, 0);
    for (int y = height - 1; y >= 0; --y) {
        const unsigned char *row_pointer = &pixels[y * width * 3];
        std::copy(row_pointer, row_pointer + width * 3, padded_row.begin());
        file.write(reinterpret_cast<char *>(padded_row.data()), row_size);
    }

    file.close();
    return true;
}

void conversion::convert_rgb_to_bgr(std::vector<unsigned char> &pixels, int width, int height) {
    int row_stride = width * 3; // Nombre d'octets par ligne (3 octets par pixel pour RGB)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char *pixel = &pixels[y * row_stride + x * 3];
            std::swap(pixel[0], pixel[2]); // Échanger le rouge (R) et le bleu (B)
        }
    }
}

bool conversion::png_to_jpeg(const std::string& png_path, const std::string& jpeg_path, int quality) {
    FILE* infile = fopen(png_path.c_str(), "rb");
    FILE* outfile = fopen(jpeg_path.c_str(), "wb");

    if (!infile || !outfile) {
        std::cerr << "Erreur : Impossible d'ouvrir les fichiers.\n";
        if (infile) fclose(infile);
        if (outfile) fclose(outfile);
        return false;
    }

    // Initialisation PNG
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (!png || !info) {
        std::cerr << "Erreur : Impossible d'initialiser libpng.\n";
        fclose(infile);
        fclose(outfile);
        return false;
    }
    png_init_io(png, infile);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGBA) png_set_strip_alpha(png);

    png_read_update_info(png, info);

    // Décompression JPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3; // RGB
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    // Conversion ligne par ligne
    unsigned char* row = new unsigned char[png_get_rowbytes(png, info)];
    while (cinfo.next_scanline < cinfo.image_height) {
        png_read_row(png, row, nullptr);
        jpeg_write_scanlines(&cinfo, &row, 1);
    }

    // Nettoyage
    delete[] row;
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(infile);
    fclose(outfile);

    return true;
}

bool conversion::jpeg_to_png(const std::string& jpeg_path, const std::string& png_path) {
    FILE* infile = fopen(jpeg_path.c_str(), "rb");
    FILE* outfile = fopen(png_path.c_str(), "wb");

    if (!infile || !outfile) {
        std::cerr << "Erreur : Impossible d'ouvrir les fichiers.\n";
        if (infile) fclose(infile);
        if (outfile) fclose(outfile);
        return false;
    }

    // Décompression JPEG
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // Initialisation PNG
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (!png || !info) {
        std::cerr << "Erreur : Impossible d'initialiser libpng.\n";
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        fclose(outfile);
        return false;
    }
    png_init_io(png, outfile);
    png_set_IHDR(png, info, cinfo.output_width, cinfo.output_height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // Conversion ligne par ligne
    unsigned char* row = new unsigned char[cinfo.output_width * cinfo.output_components];
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &row, 1);
        png_write_row(png, row);
    }

    // Nettoyage
    delete[] row;
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    fclose(outfile);

    return true;
}

bool conversion::png_to_bmp(const std::string& png_path, const std::string& bmp_path) {
    // Ouvrir le fichier PNG en lecture
    FILE* fp = fopen(png_path.c_str(), "rb");
    if (!fp) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier PNG." << std::endl;
        return false;
    }

    // Initialisation de libpng
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    // Récupération des dimensions et du type d'image
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Configuration pour garantir un format standard
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

    // Suppression du canal alpha si présent (BMP ne supporte pas l'alpha)
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_strip_alpha(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    // Allocation des lignes pour les données d'image
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < height; ++y) {
        row_pointers[y] = new png_byte[rowbytes];
    }

    png_read_image(png_ptr, row_pointers.data());
    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    // Ouvrir le fichier BMP en écriture
    FILE* bmp_fp = fopen(bmp_path.c_str(), "wb");
    if (!bmp_fp) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier BMP en écriture." << std::endl;
        for (auto row : row_pointers) delete[] row;
        return false;
    }

    // Création de l'en-tête BMP
    int row_size = (3 * width + 3) & ~3; // Alignement sur 4 octets
    int file_size = 54 + row_size * height;

    unsigned char bmp_header[54] = {0};
    bmp_header[0] = 'B';
    bmp_header[1] = 'M';
    *reinterpret_cast<int*>(&bmp_header[2]) = file_size;
    *reinterpret_cast<int*>(&bmp_header[10]) = 54;
    *reinterpret_cast<int*>(&bmp_header[14]) = 40;
    *reinterpret_cast<int*>(&bmp_header[18]) = width;
    *reinterpret_cast<int*>(&bmp_header[22]) = height;
    *reinterpret_cast<short*>(&bmp_header[26]) = 1;
    *reinterpret_cast<short*>(&bmp_header[28]) = 24;

    fwrite(bmp_header, sizeof(bmp_header), 1, bmp_fp);

    // Écriture des pixels (conversion de RGB en BGR)
    std::vector<unsigned char> padding(row_size - 3 * width, 0);
    for (int y = height - 1; y >= 0; --y) { // BMP stocke les lignes de bas en haut
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; ++x) {
            unsigned char r = row[x * 3 + 0];
            unsigned char g = row[x * 3 + 1];
            unsigned char b = row[x * 3 + 2];
            fwrite(&b, 1, 1, bmp_fp);
            fwrite(&g, 1, 1, bmp_fp);
            fwrite(&r, 1, 1, bmp_fp);
        }
        fwrite(padding.data(), 1, padding.size(), bmp_fp);
    }

    fclose(bmp_fp);

    for (auto row : row_pointers) {
        delete[] row;
    }

    return true;
}

bool conversion::bmp_to_png(const std::string& bmp_path, const std::string& png_path) {
    FILE* bmp_fp = fopen(bmp_path.c_str(), "rb");
    if (!bmp_fp) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier BMP." << std::endl;
        return false;
    }

    // Lire l'en-tête BMP
    unsigned char bmp_header[54];
    if (fread(bmp_header, sizeof(bmp_header), 1, bmp_fp) != 1) {
        std::cerr << "Erreur : Impossible de lire l'en-tête BMP." << std::endl;
        fclose(bmp_fp);
        return false;
    }

    // Vérification du format BMP
    if (bmp_header[0] != 'B' || bmp_header[1] != 'M') {
        std::cerr << "Erreur : Ce fichier n'est pas un BMP valide." << std::endl;
        fclose(bmp_fp);
        return false;
    }

    // Extraction des informations importantes
    int width = *reinterpret_cast<int*>(&bmp_header[18]);
    int height = *reinterpret_cast<int*>(&bmp_header[22]);
    short bpp = *reinterpret_cast<short*>(&bmp_header[28]);

    if (bpp != 24) {
        std::cerr << "Erreur : Seuls les fichiers BMP 24 bits sont supportés." << std::endl;
        fclose(bmp_fp);
        return false;
    }

    // Calcul de la taille des lignes (alignées sur 4 octets)
    int row_size = (3 * width + 3) & ~3;

    // Lire les données de pixel
    std::vector<unsigned char> bmp_data(row_size * height);
    if (fread(bmp_data.data(), 1, bmp_data.size(), bmp_fp) != bmp_data.size()) {
        std::cerr << "Erreur : Impossible de lire les données de pixel BMP." << std::endl;
        fclose(bmp_fp);
        return false;
    }

    fclose(bmp_fp);

    // Initialiser libpng pour écrire le fichier PNG
    FILE* png_fp = fopen(png_path.c_str(), "wb");
    if (!png_fp) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier PNG en écriture." << std::endl;
        return false;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(png_fp);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(png_fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(png_fp);
        return false;
    }

    png_init_io(png_ptr, png_fp);

    // Définir les informations de l'image PNG
    png_set_IHDR(
        png_ptr,
        info_ptr,
        width,
        height,
        8,                        // Profondeur de bits
        PNG_COLOR_TYPE_RGB,       // Type de couleur
        PNG_INTERLACE_NONE,       // Pas d'entrelacement
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE
    );

    png_write_info(png_ptr, info_ptr);

    // Écriture des lignes dans l'ordre correct
    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < height; ++y) {
        row_pointers[y] = &bmp_data[(height - 1 - y) * row_size]; // Inversion des lignes (BMP est inversé)
    }

    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, nullptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(png_fp);

    return true;

}

void conversion::ConvertJPGtoPNG(std::string Input, std::string Output){
    if (jpeg_to_png(Input, Output)) {
        std::cout << "Conversion JPEG vers PNG réussie.\n";
    } else {
        std::cerr << "Échec de la conversion JPEG vers PNG.\n";
    }
}

void conversion::ConvertPNGtoJPG(std::string Input, std::string Output){
    if (png_to_jpeg(Input, Output, 90)) {
        std::cout << "Conversion PNG vers JPEG réussie.\n";
    } else {
        std::cerr << "Échec de la conversion PNG vers JPEG.\n";
    }
}

void conversion::ConvertBMPtoJPG(std::string Input, std::string Output){
    if (bmp_to_jpeg(Input, Output, 90)) {
        std::cout << "Conversion réussie !" << std::endl;
    } else {
        std::cerr << "Erreur lors de la conversion" << std::endl;
    }
}

void conversion::ConvertJPGtoBMP(std::string Input, std::string Output){
    int width, height;
    std::vector<unsigned char> pixels;

    if (!read_jpeg(Input, width, height, pixels)) {
        return;
    }


     convert_rgb_to_bgr(pixels, width, height);
     
    if (write_bmp(Output, width, height, pixels)) {
        std::cout << "Conversion réussie : " << Output << std::endl;
    } else {
        std::cerr << "Erreur lors de l'écriture du BMP" << std::endl;
        return;
    }
}

void conversion::ConvertPNGtoBMP(std::string Input, std::string Output){
     std::cout << "Conversion de PNG vers BMP..." << std::endl;
    if (png_to_bmp(Input, Output)) {
        std::cout << "PNG vers BMP réussi : " << Output << std::endl;
    } else {
        std::cerr << "Échec de la conversion PNG vers BMP." << std::endl;
    }


}

void conversion::ConvertBMPtoPNG(std::string Input, std::string Output){
    std::cout << "Conversion de BMP vers PNG..." << std::endl;
    if (bmp_to_png(Input, Output)) {
        std::cout << "BMP vers PNG réussi : " << Output << std::endl;
    } else {
        std::cerr << "Échec de la conversion BMP vers PNG." << std::endl;
    }
}