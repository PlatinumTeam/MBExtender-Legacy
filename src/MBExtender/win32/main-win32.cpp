// MBExtender.cpp : Defines the entry point for the application.
//

#include <Windows.h>
#include <cstring>
#include "DLLInjection.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	STARTUPINFO si;
	PROCESS_INFORMATION info;
	HMODULE dll;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (!CreateProcessWithDLL("marbleblast.exe", NULL, NULL, NULL, false, 0, NULL, NULL, &si, &info, "PluginLoader.dll", &dll))
	{
		MessageBox(NULL, "Unable to start marbleblast.exe with PluginLoader.dll injected.\r\nMake sure both files exist and then try again.", "MBExtender", MB_OK | MB_ICONERROR);
		return 0;
	}

	DWORD result = 0;
	bool injected = CallRemoteDLLFunction(info.hProcess, dll, "PluginLoader.dll", "initPluginLoader", NULL, 0, &result);
	if (!injected)
		MessageBox(NULL, "Unable to initialize the plugin loader.\r\nMake sure PluginLoader.dll is up to date and then try again.", "MBExtender", MB_OK | MB_ICONERROR);
	if (!injected || !result)
	{
		TerminateProcess(info.hProcess, 0);
		CloseHandle(info.hProcess);
		CloseHandle(info.hThread);
		return 0;
	}
	
	ResumeThread(info.hThread);
	CloseHandle(info.hProcess);
	CloseHandle(info.hThread);
	return 0;
}