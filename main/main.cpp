#include <stdio.h>
#include <string>


#include "CTfLiteClass.h"
#include "Helper.h"

#include "dig-01.h"


extern "C" void app_main(void)
{
    CTfLiteClass* neuralnetwork;
    float result;
    std::string image_file;

    printf("Prüfe SD-Karte und PSRAM Verfügbarkeit.\n");

    if (!Init_SDCard_GPIO())
        return;
    CheckPSRAM();
    printf("\n");

    // Initialisiere Klasse
    neuralnetwork = new CTfLiteClass;


    /*
    Die Beschreibung des neuronalen Netzes kann entweder von der tflite-Datei von der SD-Karte
    geladen werden oder als CharArray aus der Header-Datei (siehe include) eingebunden werden.

    Die entsprechende Optin muss unten ausgewählt werden:
    */
    printf("Lade tflite-Model\n");

    neuralnetwork->LoadModelFromFile("/sdcard/dig-01.tfl");
//    neuralnetwork->LoadModelFromCharArray(tflite_model);

    printf("\nGröße Eingangs-Layer:\n");
    neuralnetwork->GetInputDimension();
    printf("\n\nGröße Ausgangs-Layer:\n");
    neuralnetwork->GetOutputDimension();
    printf("\n");

    printf("Lade Bilddaten...\n");
    image_file = "/sdcard/number3.bmp";
    neuralnetwork->LoadInputImage(image_file.c_str());

    printf("Berechne neuronales Netz ...\n\n");
    neuralnetwork->Invoke();

    printf("Frage Ergebnis ab ...\n");
    printf("Filename: %s\n", image_file.c_str());
    printf(" Einzelne Output-Neuronen:\n");
    for (int _output = 0; _output < 11; _output++)
    {
        result = neuralnetwork->GetOutputValue(_output);
        printf("   Neuron #%d: %.1f\n", _output, result);
    }
    result = neuralnetwork->GetOutClassification();
 
    printf("  CNN-Klassifizierung: %d\n", (int) result);

    printf("\n\nLade Bilddaten...\n");
    image_file = "/sdcard/number2.jpg";
    neuralnetwork->LoadInputImage(image_file.c_str());

    printf("Berechne neuronales Netz ...\n\n");
    neuralnetwork->Invoke();

    printf("Frage Ergebnis ab ...\n");
    result = neuralnetwork->GetOutClassification();
 
    printf("%s: CNN-Klassifizierung: %d\n", image_file.c_str(), (int) result);

    printf("\nHerzlichen Glückwunsch!\n");
}
