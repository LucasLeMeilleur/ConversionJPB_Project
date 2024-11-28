#include "conversion.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {

    Conversion Convert;
    
    // Vérification du nombre d'arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <extension1> <extension2> \n";
        return 1;
    }

    std::string FileInput = argv[1], FileOutput = argv[2];
    std::string Extension[3] = {"png", "jpg", "bmp"};
    bool File1 = false, File2 = false;


    if(FileInput.length() < 5 || FileOutput.length() < 5){
        std::cerr << "Nom des fichier trop court";
        return 1;
    }

    std::string ExtInput = FileInput.substr(FileInput.length() -3);
    std::string ExtOutput = FileOutput.substr(FileOutput.length() -3);
    
    for (int i = 0; i < 3; i++){
        if(ExtInput == Extension[i]) File1 = true;
        if(ExtOutput == Extension[i]) File2 = true;
    }
    
    if(!File1 or !File2){
        std::cerr << "Extension non prise en charge";
    }
    
    if(ExtInput == "png" && ExtOutput == "jpg") Convert.ConvertPNGtoJPG(FileInput, FileOutput); 
    else if(ExtInput == "jpg" && ExtOutput == "png") Convert.ConvertJPGtoPNG(FileInput, FileOutput); 
    else if(ExtInput == "png" && ExtOutput == "bmp") Convert.ConvertPNGtoBMP(FileInput, FileOutput); 
    else if(ExtInput == "bmp" && ExtOutput == "png") Convert.ConvertBMPtoPNG(FileInput, FileOutput); 
    else if(ExtInput == "jpg" && ExtOutput == "bmp") Convert.ConvertJPGtoBMP(FileInput, FileOutput); 
    else if(ExtInput == "bmp" && ExtOutput == "jpg") Convert.ConvertBMPtoJPG(FileInput, FileOutput); 
    else {
        std::cerr << "Extensions inconnues ou similaires";
        return 1;
    }

    return 0;
}
