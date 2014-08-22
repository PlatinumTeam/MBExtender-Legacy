#include <Windows.h>

typedef DWORD (WINAPI *initPluginLoader_ptr)(LPVOID unused);

bool injectPluginLoader()
{
	HMODULE loader = LoadLibrary("PluginLoader.dll");
	if (!loader)
	{
		MessageBox(NULL, "Unable to load PluginLoader.dll.\r\nMake sure the file exists and then try again.", "MBExtender", MB_OK | MB_ICONERROR);
		return false;
	}
	initPluginLoader_ptr initPluginLoader = reinterpret_cast<initPluginLoader_ptr>(GetProcAddress(loader, "initPluginLoader"));
	if (!initPluginLoader)
	{
		FreeLibrary(loader);
		MessageBox(NULL, "Unable to locate initPluginLoader in PluginLoader.dll.\r\nVerify that the file is not corrupt and then try again.", "MBExtender", MB_OK | MB_ICONERROR);
		return false;
	}
	return (initPluginLoader(nullptr) != 0);
}