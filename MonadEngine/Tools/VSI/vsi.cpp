// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#define NOMINMAX
#define NODRAWTEXT
#define NOBITMAP
#define NODRAWTEXT
#define NOGDI
#define NOHELP
#define NOMCX
#define NOSERVICE
#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <Windows.h>

using namespace std;

#define INPUT_LAYOUT (argv[1])
#define INPUT_FILE (argv[2])
#define OUTPUT_FILE (argv[3])
#define EXIT_ON_ERROR(ERR_MSG) { DeleteFileA(OUTPUT_FILE); OutputDebugStringA("Error: "); OutputDebugStringA(ERR_MSG); return EXIT_FAILURE; }

int main(
	int argc,
	char* argv[]
)
{
	if (4 != argc)
		EXIT_ON_ERROR("wrong number of parameters.");
	try
	{
		ofstream file(OUTPUT_FILE, ios::out | ios::trunc | ios::binary);
		if (file.is_open()) [[likely]]
		{
			file.exceptions(ofstream::failbit | ofstream::badbit);
			file.write(INPUT_LAYOUT, strlen(INPUT_LAYOUT) + 1);
		}
		else [[unlikely]]
			EXIT_ON_ERROR("dest. file not opened.");

		streampos size(0);
		unique_ptr<char[]> memblock;
		if (ifstream fileIn(INPUT_FILE, ios::in | ios::binary | ios::ate); file.is_open())
		{
			fileIn.exceptions(ifstream::failbit | ifstream::badbit);
			size = fileIn.tellg();
			memblock = make_unique_for_overwrite<char[]>(size);
			fileIn.seekg(0, ios::beg);
			fileIn.read(memblock.get(), size);
		}
		else
		{
			file.close();
			EXIT_ON_ERROR("source file not opened.");
		}

		file.write(memblock.get(), size);
	}
	catch (const exception& e)
	{
		EXIT_ON_ERROR(e.what());
	}
	return ERROR_SUCCESS;
}