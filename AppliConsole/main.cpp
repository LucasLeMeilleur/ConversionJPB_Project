#include <iostream>
#include <fstream>
#include <vector>
#include "conversion.h"
#include <chrono>
#include <thread>

int main() {
    conversion Convert;

    // std::cout << "Conversion terminée !" << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    // Convert.ConvertPNGtoBMP("Pol2.png", "Pol2.bmp");
    // Convert.ConvertBMPtoPNG("Pol2.bmp", "Pol.png");
    while (true) {
        std::string extInput, extOutput;
        std::string fichierInput = "", fichierOutput = "src/image";

        // Demander l'extension d'entrée
        std::cout << "Quel est l'extension d'entrée ? (jpg/bmp/png) : ";
        std::cin >> extInput;
        std::cout << std::endl;

        // Vérifier l'extension d'entrée
        if (extInput != "jpg" && extInput != "bmp" && extInput != "png") {
            std::cout << "Extension d'entrée invalide ou non prise en charge !" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        // Demander le nom du fichier d'entrée
        std::cout << "Le nom de votre fichier (sans extension) : ";
        std::cin >> fichierInput;
        std::cout << std::endl;

        // Ajouter l'extension d'entrée au nom de fichier
        fichierInput += "." + extInput;

        // Demander l'extension de sortie
        std::cout << "Extension de sortie (jpg/bmp) : ";
        std::cin >> extOutput;
        std::cout << std::endl;

        // Vérifier l'extension de sortie
        if (extOutput != "jpg" && extOutput != "bmp" && extOutput != "png") {
            std::cout << "Extension de sortie non prise en charge !" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        // Vérifier si les extensions d'entrée et de sortie sont les mêmes
        if (extInput == extOutput){
            std::cout << "Le fichier a la même extension, aucune conversion nécessaire." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        // Ajouter l'extension de sortie au nom de fichier de sortie
        fichierOutput += "." + extOutput;

        // Effectuer la conversion
        if (extInput == "bmp" && extOutput == "jpg") {
            Convert.ConvertBMPtoJPG(fichierInput, fichierOutput);
        } else if (extInput == "jpg" && extOutput == "bmp") {
            Convert.ConvertJPGtoBMP(fichierInput, fichierOutput);
        } else if (extInput == "png" && extOutput == "jpg"){
            Convert.ConvertPNGtoJPG(fichierInput, fichierOutput);
        } else if (extInput == "jpg" && extOutput == "png"){
            Convert.ConvertJPGtoPNG(fichierInput, fichierOutput);
        } else if (extInput == "png" && extOutput == "bmp"){
            Convert.ConvertPNGtoBMP(fichierInput,fichierOutput);
        } else if (extInput == "bmp" && extOutput == "png"){
            Convert.ConvertBMPtoPNG(fichierInput, fichierOutput);
        }

        
    }

    return 0;
}
