// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

namespace
{
	bool verifyGame();
}

void installHooks();

namespace
{
	extern "C" __declspec(dllexport) bool initPluginLoader()
	{
		if (!verifyGame())
		{
			MessageBox(NULL, "MBExtender is only compatible with the full version of Marble Blast Gold.", "MBExtender", MB_OK | MB_ICONERROR);
			return false;
		}

		installHooks();
		return true;
	}

	bool verifyGame()
	{
		const char *testPointer = reinterpret_cast<const char*>(0x6796C4);
		const char *testStr = "Marble Blast";

		// Make sure we can actually read from the test memory location
		MEMORY_BASIC_INFORMATION memInfo;
		if (VirtualQuery(testPointer, &memInfo, sizeof(memInfo)) != sizeof(memInfo))
			return false;
		if (memInfo.Protect == 0 || (memInfo.Protect & PAGE_NOACCESS) || (memInfo.Protect & PAGE_EXECUTE))
			return false;

		// Check if the string matches
		if (memcmp(testPointer, testStr, strlen(testStr)) != 0)
			return false;
		return true;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		if (!initPluginLoader())
			TerminateProcess(GetCurrentProcess(), 0);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
