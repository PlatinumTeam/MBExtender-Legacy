#include <iostream>
#include <cstring>
#include "PEFile.h"

const char *DefaultSuffix = "-patched";        // EXE suffix to append when only one argument is passed
char *const DllName = "PluginLoader.dll";      // DLL to load
char *DllFunctions[] = { "initPluginLoader" }; // Functions to add

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		std::cout << "MBGPatcher: Modifies the Marble Blast Gold EXE to load " << DllName << std::endl;
		std::cout << "Usage: MBGPatcher <path to Marble Blast Gold EXE> [path to output EXE]" << std::endl;
		return 1;
	}

	char *inputPath = argv[1];
	std::string outputPath;
	if (argc == 3)
	{
		// Take the output path from the command line
		outputPath = argv[2];
	}
	else
	{
		// Append the suffix onto the end of the EXE name
		outputPath = inputPath;
		size_t extensionStart = outputPath.find('.');
		if (extensionStart != std::string::npos)
			outputPath.insert(extensionStart, DefaultSuffix);
		else
			outputPath += DefaultSuffix;
	}

	// Load the input file
	PEFile inputFile;
	bool result = inputFile.loadFromFile(inputPath);
	if (!result)
	{
		std::cerr << "Error: invalid EXE file!" << std::endl;
		return 2;
	}

	// Make sure the DLL isn't already imported
	PE_IMPORT_DLL *importTable = &inputFile.importTable;
	while (importTable)
	{
		if (strcmp(importTable->DllName, DllName) == 0)
		{
			std::cerr << "Error: the input EXE file already loads " << DllName << "!" << std::endl;
			return 3;
		}
		importTable = importTable->Next;
	}

	// Add the import and save the file
	inputFile.addImport(DllName, DllFunctions, sizeof(DllFunctions) / sizeof(DllFunctions[0]));
	if (!inputFile.saveToFile(&outputPath[0]))
	{
		std::cerr << "Error: failed to save the output file!" << std::endl;
		return 4;
	}
	
	std::cout << "EXE patched successfully!" << std::endl;
	return 0;
}