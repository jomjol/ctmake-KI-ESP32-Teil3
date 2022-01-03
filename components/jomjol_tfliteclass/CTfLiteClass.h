
#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "esp_err.h"
#include "esp_log.h"


class CTfLiteClass
{
    protected:
        tflite::ErrorReporter *error_reporter;
        const tflite::Model* model;
        tflite::MicroInterpreter* interpreter;
        TfLiteTensor* output = nullptr;     
        static tflite::AllOpsResolver resolver;

        unsigned char *LoadedModelDescription_tflite = NULL;

        int kTensorArenaSize;
        uint8_t *tensor_arena;

        float* input;
        int input_i;
        int im_height, im_width, im_channel;

        long GetFileSize(std::string filename);
        unsigned char* ReadFileToCharArray(std::string _fn);
        
    public:
        CTfLiteClass();
        ~CTfLiteClass();        
        void LoadModelFromFile(std::string _fn);
        void LoadModelFromCharArray(unsigned char *_input);
        void MakeAllocate();
        void GetInputTensorSize();
        bool LoadInputImage(std::string _fn);
        void Invoke();
        void GetOutPut();
        int GetOutClassification();
        int GetClassFromImage(std::string _fn);

        float GetOutputValue(int nr);
        void GetInputDimension(bool silent = false);
        void GetOutputDimension(bool silent = false);

};
