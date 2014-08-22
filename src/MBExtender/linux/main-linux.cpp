#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <unistd.h>

namespace
{
	bool envAddItem(const char *var, const char *item);
	bool addPreload(const char *path);
	void showError(const char *msg);
}

int main(int argc, const char *argv[])
{
	std::cout << "Launching game..." << std::endl;

	errno = 0;
	if (!addPreload("./PluginLoader.so"))
	{
		showError("Failed to initialize LD_PRELOAD");
		return 1;
	}
	
	execl("./marbleblastgold.bin", "marbleblastgold.bin", nullptr);
	showError("Failed to launch the game");
	return 1;
}

namespace
{
	bool envAddItem(const char *var, const char *item)
	{
		char *oldValue = getenv(var);
		if (!oldValue)
			return (setenv(var, item, true) == 0);
		char *newValue = new char[strlen(oldValue) + 1 + strlen(item)];
		sprintf(newValue, "%s:%s", item, oldValue);
		int result = setenv(var, newValue, true);
		delete[] newValue;
		return (result == 0);
	}

	bool addPreload(const char *path)
	{
		return envAddItem("LD_PRELOAD", path);
	}

	void showError(const char *msg)
	{
		if (errno)
			perror(msg);
		else
			std::cerr << msg << std::endl;
	}
}
