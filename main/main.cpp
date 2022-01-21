#include <stdio.h>
#include <string>

#include "CTfLiteClass.h"
#include "Helper.h"

#include "dig-01.h"

void ErweitertesBeispiel();

extern "C" void app_main(void)
{
    CTfLiteClass* neuralnetwork;
    float result;
    int AnzahlOutputNeurons;
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

    neuralnetwork->LoadModelFromFile("/sdcard/dig-01.tfl");         // dynamisch
//    neuralnetwork->LoadModelFromCharArray(tflite_model);          // statisch

    printf("\nGröße Eingangs-Layer:\n");
    neuralnetwork->GetInputDimension();
    printf("\n\nGröße Ausgangs-Layer:\n");
    AnzahlOutputNeurons = neuralnetwork->GetOutputDimension();
    printf("\n");

    printf("Lade Bilddaten...\n");
    image_file = "/sdcard/number3.bmp";
    neuralnetwork->LoadInputImage(image_file.c_str());

    printf("Berechne neuronales Netz ...\n\n");
    neuralnetwork->Invoke();

    printf("Frage Ergebnis ab ...\n");
    printf("Filename: %s\n", image_file.c_str());
    printf(" Einzelne Output-Neuronen:\n");
    for (int _output = 0; _output < AnzahlOutputNeurons; _output++)
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

    printf("\nHerzlichen Glückwunsch!\n\n\n");
    delete neuralnetwork;       // Freigeben des Speichers




    ////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////// Ab hier beginnt ein ausführlicheres Beispiel /////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////

    EnableConsoleInputMode();
    printf("Drücke beliebige Taste zum Fortfahren ... (erweitertes Beispiel)\n");    
    getchar();

    ErweitertesBeispiel();

}


void ErweitertesBeispiel()
{
    CTfLiteClass *classtflite;

    float result;
    uint32_t starttime, stoptime, loadtime, calctime, loadtime_sum, calctime_sum;
    std::string name_tflite, name_image;
    int tflite, image;
    std::string neuralnetwork_files[3] = {"small.tfl", "middle.tfl", "large.tfl"};

    for (tflite = 0; tflite < 3; ++tflite)
    { 
        printf("\n====== Model %s ====================\n  ", neuralnetwork_files[tflite].c_str());
        starttime = esp_log_timestamp();
        classtflite = new CTfLiteClass; 
        classtflite->LoadModelFromFile("/sdcard/ext_exam/" + neuralnetwork_files[tflite]); 
        stoptime = esp_log_timestamp();
        printf("Model loading time: %dms\n  ", stoptime-starttime);
        loadtime_sum = 0; 
        calctime_sum = 0;
        starttime = stoptime;

        for (image = 0; image < 5; ++image)
        {
            name_image = "/sdcard/digit" + std::to_string(image) + ".bmp";

            classtflite->LoadInputImage(name_image);

            stoptime = esp_log_timestamp(); 
            loadtime = stoptime - starttime; starttime = stoptime;

            classtflite->Invoke();

            stoptime = esp_log_timestamp(); 
            calctime = stoptime - starttime; starttime = stoptime;

            result = classtflite->GetOutClassification();

            loadtime_sum += loadtime;
            calctime_sum += calctime;
            printf("Image: %s - CNN-Classification: %d - Loadtime: %dms, Calctime: %dms\n  ", name_image.c_str(), (int) result, loadtime, calctime);
        }

        printf("Average Loadtime: %dms, Calctime: %dms\n", (int) (loadtime_sum/5),  (int) (calctime_sum/5)); 

        delete classtflite;
    }

}
