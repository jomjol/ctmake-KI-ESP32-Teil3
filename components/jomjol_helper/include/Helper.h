#include <string>
#include <fstream>

using namespace std;


bool Init_SDCard_GPIO(bool _onebitmode = false);

bool CheckPSRAM();

void EnableConsoleInputMode();


string getFileType(string filename);
string toUpper(string in);

