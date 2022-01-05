#include "CTfLiteClass.h"
#include "stb_image.h"

#include <sys/stat.h>

bool debugdetailtflite = false;

float CTfLiteClass::GetOutputValue(int nr)
{
		TfLiteTensor* output2 = this->interpreter->output(0);

		int numeroutput = output2->dims->data[1];
		if ((nr+1) > numeroutput)
			return -1000;

		return output2->data.f[nr];
}


int CTfLiteClass::GetClassFromImage(std::string _fn)
{
//  printf("Before Load image %s\n", _fn.c_str());
		if (!LoadInputImage(_fn))
			return -1000;
//  printf("After Load image %s\n", _fn.c_str());

		Invoke();
	printf("After Invoke %s\n", _fn.c_str());

		return GetOutClassification();
//    return 0;
}

int CTfLiteClass::GetOutClassification()
{
//  printf("test\n");


	float zw_max = 0;
	float zw;
	int zw_class = -1;

	TfLiteTensor* output2 = interpreter->output(0);
	if (output2 == NULL)
		return -1;

	int numeroutput = output2->dims->data[1];
//  printf("Anzahl Output: %d\n", numeroutput);
	for (int i = 0; i < numeroutput; ++i)
	{
		zw = output2->data.f[i];
		if (zw > zw_max)
		{
				zw_max = zw;
				zw_class = i;
		}
	}
//  printf("Result Ziffer: %d\n", zw_class);       
	return zw_class;
}

void CTfLiteClass::GetInputDimension(bool silent)
{
	TfLiteTensor* input2 = this->interpreter->input(0);

	int numdim = input2->dims->size;
	if (!silent)  printf("Anzahl Ausgangsdimensionen: %d\n", numdim - 1);  

	int sizeofdim;
	for (int j = 1; j < numdim; ++j)
	{
		sizeofdim = input2->dims->data[j];
		if (!silent) printf("  Größe Dimension %d: %d\n", j, sizeofdim);  
		if (j == 1) im_height = sizeofdim;
		if (j == 2) im_width = sizeofdim;
		if (j == 3) im_channel = sizeofdim;
	}
}

void CTfLiteClass::GetOutputDimension(bool silent)
{
	TfLiteTensor* output2 = this->interpreter->output(0);

	int numdim = output2->dims->size;
	printf("Anzahl Ausgangsdimensionen: %d\n", numdim - 1);  

	int sizeofdim;
	for (int j = 1; j < numdim; ++j)
	{
		sizeofdim = output2->dims->data[j];
		printf("  Größe Dimension %d: %d\n", j, sizeofdim);  
	}
}

void CTfLiteClass::GetOutPut()
{
	TfLiteTensor* output2 = this->interpreter->output(0);

	int numdim = output2->dims->size;
	printf("NumDimension: %d\n", numdim);  

	int sizeofdim;
	for (int j = 0; j < numdim; ++j)
	{
		sizeofdim = output2->dims->data[j];
		printf("SizeOfDimension %d: %d\n", j, sizeofdim);  
	}


	float fo;

	// Process the inference results.
	int numeroutput = output2->dims->data[1];
	for (int i = 0; i < numeroutput; ++i)
	{
	 fo = output2->data.f[i];
		printf("Result %d: %f\n", i, fo);  
	}
}

void CTfLiteClass::Invoke()
{
	interpreter->Invoke();
}


bool CTfLiteClass::LoadInputImage(std::string _fn)
{
	int width, height, bpp; 
	int channels = 3;
	uint8_t* rgb_image;
	stbi_uc* p_source;
	unsigned char red, green, blue;
	
	rgb_image = stbi_load(_fn.c_str(), &width, &height, &bpp, channels);

	if ((width != im_width) || (height != im_height)) 
	{
		printf("Bildgröße passt nicht zum neuronalen Netz !!!\n");
		printf(" Image    : %d x %d (%s)\n", width, height, _fn.c_str());
		printf(" CNN-Größe: %d x %d\n", im_width, im_height);
		return false;
	}

	input_i = 0;
	float* input_data_ptr = (interpreter->input(0))->data.f;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			{
					p_source = rgb_image + (channels * (y * width + x));
					red = p_source[0];
					green = p_source[1];
					blue = p_source[2];
					*(input_data_ptr) = (float) red;
					input_data_ptr++;
					*(input_data_ptr) = (float) green;
					input_data_ptr++;
					*(input_data_ptr) = (float) blue;
					input_data_ptr++;
			}

	return true;
}


void CTfLiteClass::MakeAllocate()
{
	/*
	Hier werden für die verschiedenen Operator (die neuronalen Layer, z.B. Conv2d, MaxPooling, Full, ...) geladen.
	Man kann hier jeden benötigten einzelnen Operator laden. Dadurch wird Speicherplatz gespart. Hier wird der
	allgemeine Operator "AllOpsResolver" geladen, der alle verfügbaren Operatoren abdeckt.
	*/
	static tflite::AllOpsResolver resolver;
	interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) {
				TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
		GetInputDimension();   
		return;
	}

}

void CTfLiteClass::GetInputTensorSize(){
		float *zw = this->input;
		int test = sizeof(zw);
		printf("Input Tensor Dimension: %d\n", test);       

		printf("Input Tensor Dimension: %d\n", test);   
}

long CTfLiteClass::GetFileSize(std::string filename)
{
		struct stat stat_buf;
		long rc = stat(filename.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
}


unsigned char* CTfLiteClass::ReadFileToCharArray(std::string _fn)
{
		long size;
		
		size = GetFileSize(_fn);

		if (size == -1)
		{
		printf("\nFile existiert nicht.\n");
				return NULL;
		}


	unsigned char *result = (unsigned char*) malloc(size);
	
	if(result != NULL) {
//		printf("\nSpeicher ist reserviert: %ld\n", size);
				FILE* f = fopen(_fn.c_str(), "rb");     // vorher  nur "r"
				fread(result, 1, size, f);
				fclose(f);        
	}else {
		printf("\nKein freier Speicher vorhanden.\n");
	}    
	return result;
}

void CTfLiteClass::LoadModelFromFile(std::string _fn){
	LoadedModelDescription_tflite = ReadFileToCharArray(_fn.c_str());
	LoadModelFromCharArray(LoadedModelDescription_tflite);
}

void CTfLiteClass::LoadModelFromCharArray(unsigned char *_input){
	static tflite::MicroErrorReporter micro_error_reporter;
	error_reporter = &micro_error_reporter;

	model = tflite::GetModel(_input);
	TFLITE_MINIMAL_CHECK(model != nullptr); 		

	if (model)
		MakeAllocate(); 
}



CTfLiteClass::CTfLiteClass(int _memsize)
{
	model = nullptr;
	interpreter = nullptr;
	input = nullptr;
	output = nullptr;  
	kTensorArenaSize = _memsize;
	tensor_arena = (uint8_t*) malloc(kTensorArenaSize);
//    tensor_arena = new uint8_t[kTensorArenaSize]; 
}

CTfLiteClass::~CTfLiteClass()
{
	delete this->tensor_arena;
	delete this->interpreter;
	delete this->error_reporter;
	if (LoadedModelDescription_tflite)
		free(LoadedModelDescription_tflite);
}        
