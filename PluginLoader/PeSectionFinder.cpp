#include <Windows.h>
#include <cstdint>
#include "PeSectionFinder.h"

namespace CodeInjection
{
	PeSectionFinder::PeSectionFinder()
	{
		init(GetModuleHandle(NULL));
	}

	PeSectionFinder::PeSectionFinder(HMODULE module)
	{
		init(module);
	}

	void PeSectionFinder::init(HMODULE module)
	{
		image = module;
		ntHeaders = nullptr;
		sections = nullptr;

		// Get a pointer to the DOS header by just casting the module handle
		auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return; // Bad DOS header

		// Get a pointer to the NT header by adding e_lfanew to the DOS header address
		ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<uint8_t*>(module) + dosHeader->e_lfanew);
		if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			// Bad NT header
			ntHeaders = nullptr;
			return;
		}

		// The section table immediately follows the NT headers
		sections = reinterpret_cast<IMAGE_SECTION_HEADER*>(ntHeaders + 1);
	}

	bool PeSectionFinder::findSection(const char *name, PeSectionInfo *result)
	{
		for (auto i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
		{
			auto section = &sections[i];
			auto sectionName = reinterpret_cast<const char*>(section->Name);
			if (strncmp(sectionName, name, 8) == 0)
			{
				result->start = reinterpret_cast<void*>(ntHeaders->OptionalHeader.ImageBase + section->VirtualAddress);
				result->physicalSize = section->SizeOfRawData;
				result->virtualSize = section->Misc.VirtualSize;
				return true;
			}
		}
		return false;
	}
}