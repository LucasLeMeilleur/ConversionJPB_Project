#include "cpp/conversion.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {


    
    // Vérification du nombre d'arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <extension1> <extension2> <id>\n";
        return 1;
    }


    std::string arg1 = argv[1], arg2 = argv[2], arg3 = argv[3];


    // Liste des extensions supportées
    std::vector<std::string> extensions = {"jpg", "png", "bmp"};
    bool Param1 = false, Param2 = false;

    // Vérifier si les arguments sont valides
    for (const auto &ext : extensions) {
        if (arg1 == ext) Param1 = true;
        if (arg2 == ext) Param2 = true;
    }

    if (!Param1) {
        std::cerr << "Erreur : Extension 1 (" << arg1 << ") non supportée !\n";
        return 1;
    }
    if (!Param2) {
        std::cerr << "Erreur : Extension 2 (" << arg2 << ") non supportée !\n";
        return 1;
    }
    if (arg1 == arg2) {
        std::cerr << "Erreur : Les extensions ne doivent pas être identiques !\n";
        return 1;
    }

    // Conversion
    Conversion Convert;
    try {
        if (arg1 == "png" && arg2 == "jpg") {
            Convert.ConvertPNGtoJPG("TempImg/Input-"+arg3+".png", "TempImg/Output-"+arg3+".jpg");
        } else if (arg1 == "jpg" && arg2 == "png"){
            Convert.ConvertJPGtoPNG("TempImg/Input-"+arg3+".jpg", "TempImg/Output-"+arg3+".png");
        } else if (arg1 == "png" && arg2 == "bmp"){
            Convert.ConvertPNGtoBMP("TempImg/Input-"+arg3+".png", "TempImg/Output-"+arg3+".bmp");
        } else if (arg1 == "bmp" && arg2 == "png"){
            Convert.ConvertBMPtoPNG("TempImg/Input-"+arg3+".bmp", "TempImg/Output-"+arg3+".png");
        } else if (arg1 == "jpg" && arg2 == "bmp"){
            Convert.ConvertJPGtoBMP("TempImg/Input-"+arg3+".jpg", "TempImg/Output-"+arg3+".bmp");
        } else if (arg1 == "bmp" && arg2 == "jpg"){
            Convert.ConvertBMPtoJPG("TempImg/Input-"+arg3+".bmp", "TempImg/Output-"+arg3+".jpg");
        } else {
            std::cerr << "Erreur : Conversion de " << arg1 << " vers " << arg2 << " non implémentée.\n";
            return 1;
        } 
    } catch (const std::exception &e) {
        std::cerr << "Erreur lors de la conversion : " << e.what() << "\n";
        return 1;
    }

    return 0;
}
