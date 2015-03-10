#include "../Filesystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

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
			struct stat st;
			if (stat(path.c_str(), &st))
				return false;
			return (S_ISDIR(st.st_mode) != 0);
		}

		/// <summary>
		/// Enumerates through the files in a directory.
		/// </summary>
		/// <param name="path">The path to the directory to enumerate through.</param>
		/// <param name="results">The vector to store resulting paths to.</param>
		/// <returns><c>true</c> if successful.</returns>
		bool enumerate(const std::string &path, std::vector<std::string> *results)
		{
			DIR *dir = opendir(path.c_str());
			if (!dir)
				return false;
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL)
			{
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				std::string entryPath = Filesystem::Path::combine(path, entry->d_name);
				results->push_back(entryPath);
			}
			closedir(dir);
			return true;
		}
	}

	namespace File
	{
		/// <summary>
		/// Determines if a file exists.
		/// </summary>
		/// <param name="path">The path to the file to check.</param>
		/// <returns><c>true</c> if the path exists and points to a file.</returns>
		bool exists(const std::string &path)
		{
			struct stat st;
			if (stat(path.c_str(), &st))
				return false;
			return (S_ISREG(st.st_mode) != 0);
		}
	}
}