#include <stdio.h>
#include <string>


#include "CTfLiteClass.h"

#include "Helper.h"

#include "dig-01.h"


extern "C" void app_main(void)
{
    CTfLiteClass neuralnetwork;
    float result;

    printf("Prüfe SD-Karte und PSRAM Verfügbarkeit.\n");

    if (!Init_SDCard_GPIO())
        return;
    CheckPSRAM();
    printf("\n");


    printf("Lade tflite-Model\n");
//    neuralnetwork.LoadModelFromFile("/sdcard/dig-01.tfl");
    neuralnetwork.LoadModelFromCharArray(tflite_model);

    printf("Bereite interne Struktur und Daten vor ...\n");
    neuralnetwork.MakeAllocate(); 


    printf("\nGröße Eingangs-Layer:\n");
    neuralnetwork.GetInputDimension();
    printf("\n\nGröße Ausgangs-Layer:\n");
    neuralnetwork.GetOutputDimension();
    printf("\n");

    printf("Lade Bilddaten...\n");
    neuralnetwork.LoadInputImage("/sdcard/number3.bmp");

    printf("Berechne neuronales Netz ...\n\n");
    neuralnetwork.Invoke();

    printf("Frage Ergebnis ab ...\n");
    result = neuralnetwork.GetOutClassification();
 
    printf("CNN-Klassifizierung: %d\n", (int) result);

    printf("\nHerzlichen Glückwunsch!\n");
}
