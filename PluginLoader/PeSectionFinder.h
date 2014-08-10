#pragma once

#include <Windows.h>

namespace CodeInjection
{
	struct PeSectionInfo
	{
		void *start;
		size_t physicalSize;
		size_t virtualSize;
	};

	class PeSectionFinder
	{
	public:
		PeSectionFinder();
		explicit PeSectionFinder(HMODULE module);
		
		bool findSection(const char *name, PeSectionInfo *result);

	private:
		void init(HMODULE module);

		HMODULE image;
		IMAGE_NT_HEADERS *ntHeaders;
		IMAGE_SECTION_HEADER *sections;
	};
}