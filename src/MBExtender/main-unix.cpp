#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#if defined(__linux)
const char *const PreloadVariable = "LD_PRELOAD";
const char *const PluginLoaderPath = "./PluginLoader.so";
const char *const GamePath = "./marbleblastgold.bin";
#elif defined(__APPLE__)
const char *const PreloadVariable = "DYLD_INSERT_LIBRARIES";
const char *const PluginLoaderPath = "./PluginLoader.dylib";
const char *const GamePath = "./MarbleBlast Gold";
#endif

namespace
{
	char *concatSeparated(const char *left, const char *right, char separator);
	bool addPreload(const char *path);
	bool envAddItem(const char *var, const char *item);
	void showError(const char *msg);
}

int main(int argc, const char *argv[])
{
	std::cout << "Launching game..." << std::endl;

#if defined(__APPLE__)
	// Get the path used to launch this executable and chdir to it
	char *exePath = strdup(argv[0]);
	char *exeDir = dirname(exePath);
	free(exePath);
	if (!exeDir)
	{
		showError("Unable to determine executable directory");
		return 1;
	}
	if (chdir(exeDir))
	{
		showError("Unable to chdir to executable directory");
		return 1;
	}
#endif

	// Set the plugin loader to be preloaded
	errno = 0;
	if (!addPreload(PluginLoaderPath))
	{
		showError("Failed to initialize LD_PRELOAD");
		return 1;
	}

	// Launch the game
	execl(GamePath, GamePath, "-nohomedir", NULL);
	showError("Failed to launch the game");
	return 1;
}

namespace
{
	char *concatSeparated(const char *left, const char *right, char separator)
	{
		char *result = new char[strlen(left) + 1 + strlen(right) + 1];
		sprintf(result, "%s%c%s", left, separator, right);
		return result;
	}

	bool addPreload(const char *path)
	{
		return envAddItem(PreloadVariable, path);
	}

	bool envAddItem(const char *var, const char *item)
	{
		// If the environment variable is not currently set, then just set its value to the item
		char *oldValue = getenv(var);
		if (!oldValue)
			return (setenv(var, item, true) == 0);

		// Otherwise, append the new value onto the current one with a colon in-between
		char *newValue = concatSeparated(oldValue, item, ':');
		int result = setenv(var, newValue, true);
		delete[] newValue;
		return (result == 0);
	}

	void showError(const char *msg)
	{
		if (errno)
			perror(msg);
		else
			std::cerr << msg << std::endl;
	}
}
