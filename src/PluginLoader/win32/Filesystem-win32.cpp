#include "../Filesystem.h"
#include <Windows.h>

namespace Filesystem
{
	namespace Directory
	{
		/// <summary>
		/// Determines if a directory exists.
		/// </summary>
		/// <param name="path">The path to the directory to check.</param>
		/// <returns><c>true</c> if the path exists and points to a directory.</returns>
		bool exists(const std::string &path)
		{
			DWORD attrib = GetFileAttributes(path.c_str());
			if (attrib == INVALID_FILE_ATTRIBUTES)
				return false;
			return ((attrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
		}

		/// <summary>
		/// Enumerates through the files in a directory.
		/// </summary>
		/// <param name="path">The path to the directory to enumerate through.</param>
		/// <param name="results">The vector to store resulting paths to.</param>
		/// <returns><c>true</c> if successful.</returns>
		bool enumerate(const std::string &path, std::vector<std::string> *results)
		{
			if (!exists(path))
				return false;
			std::string filter = Filesystem::Path::combine(path, "*");
			WIN32_FIND_DATA entry;
			HANDLE find = FindFirstFile(filter.c_str(), &entry);
			if (find == INVALID_HANDLE_VALUE)
				return (GetLastError() == ERROR_FILE_NOT_FOUND);
			do
			{
				std::string entryPath = Filesystem::Path::combine(path, entry.cFileName);
				results->push_back(entryPath);
			}
			while (FindNextFile(find, &entry));
			FindClose(find);
			return true;
		}
	}
}