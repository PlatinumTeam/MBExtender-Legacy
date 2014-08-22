#include <Windows.h>
#include "DLLInjection.h"

/// <summary>
/// Injects a DLL into another process.
/// </summary>
/// <param name="process">A handle to the process to inject the DLL into.</param>
/// <param name="dllPath">The path of the DLL to inject.</param>
/// <returns>A handle to the DLL if successful, or <c>NULL</c> otherwise.</returns>
HMODULE InjectDLLIntoProcess(HANDLE process, const char *dllPath)
{
	// Convert the DLL path string to a full path
	char fullPath[MAX_PATH];
	DWORD fullPathLen = GetFullPathNameA(dllPath, sizeof(fullPath) / sizeof(TCHAR), fullPath, NULL);
	if (fullPathLen == 0 || fullPathLen > MAX_PATH)
		return false;
	DWORD fullPathSize = fullPathLen + 1; // fullPathLen doesn't include the null terminator

	// Get the address of LoadLibraryA from kernel32.dll
	HMODULE kernel32 = GetModuleHandleA("kernel32");
	void *loadLibrary = GetProcAddress(kernel32, "LoadLibraryA");

	// Make the process call LoadLibraryA on the DLL
	HMODULE remoteDll;
	if (!CallRemoteFunction(process, loadLibrary, fullPath, fullPathSize, reinterpret_cast<DWORD*>(&remoteDll)))
		return NULL;

	return remoteDll;
}

/// <summary>
/// Calls a function in another process and waits for it to finish.
/// The function will be called from a new thread.
/// </summary>
/// <param name="process">A handle to the process to call the function in.</param>
/// <param name="function">A pointer to a function in the target process to call.</param>
/// <param name="arg">Pointer to an optional argument to copy into the target process's address space, or <c>NULL</c> for none.</param>
/// <param name="argSize">Size of the argument to copy, if any.</param>
/// <param name="result">Pointer to a DWORD to store the result to.</param>
/// <returns><c>true</c> if the call succeeded.</returns>
bool CallRemoteFunction(HANDLE process, const void *function, const void *arg, size_t argSize, DWORD *result)
{
	void *remotePayload = NULL;
	if (arg != NULL)
	{
		// Allocate pages in the target process to hold the payload
		remotePayload = VirtualAllocEx(process, NULL, argSize, MEM_COMMIT, PAGE_READWRITE);
		if (remotePayload == NULL)
			return false;

		// Copy the payload into the target process
		if (!WriteProcessMemory(process, remotePayload, arg, argSize, NULL))
		{
			VirtualFreeEx(process, remotePayload, 0, MEM_RELEASE);
			return false;
		}
	}

	// Create a remote thread which runs the function
	HANDLE thread = CreateRemoteThread(process, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(function), remotePayload, 0, NULL);
	if (thread == NULL)
	{
		if (remotePayload != NULL)
			VirtualFreeEx(process, remotePayload, 0, MEM_RELEASE);
		return false;
	}

	// Wait for the thread to finish
	WaitForSingleObject(thread, INFINITE);
	if (result != NULL)
	{
		// The thread's exit code is the function's result
		GetExitCodeThread(thread, result);
	}

	// Clean up
	CloseHandle(thread);
	if (remotePayload != NULL)
		VirtualFreeEx(process, remotePayload, 0, MEM_RELEASE);

	return true;
}


/// <summary>
/// Calls an exported function of a DLL in another process and waits for it to finish.
/// The function will be called from a new thread.
/// Note that this will temporarily load the DLL into the current process.
/// </summary>
/// <param name="process">A handle to the process to call the function in.</param>
/// <param name="remoteDll">A handle to the DLL in the remote process.</param>
/// <param name="dllPath">The path to the DLL.</param>
/// <param name="funcName">The name of the exported function to call.</param>
/// <param name="arg">Pointer to an optional argument to copy into the target process's address space, or <c>NULL</c> for none.</param>
/// <param name="argSize">Size of the argument to copy, if any.</param>
/// <param name="result">Pointer to a DWORD to store the result to.</param>
/// <returns><c>true</c> if the call succeeded.</returns>
bool CallRemoteDLLFunction(HANDLE process, HMODULE remoteDll, const char *dllPath, const char *funcName, const void *arg, const size_t argSize, DWORD *result)
{
	// Load the requested DLL into our own process
	HMODULE dll = LoadLibraryA(dllPath);
	if (dll == NULL)
		return false;

	// Locate the target function in our address space
	void *localFunction = GetProcAddress(dll, funcName);
	if (localFunction == NULL)
	{
		FreeLibrary(dll);
		return false;
	}

	// Calculate the address of the function in the target process
	// This can be done by subtracting the address in our process from the module base address,
	// and then adding that difference to the base address in the target process
	ptrdiff_t offset = reinterpret_cast<size_t>(localFunction) - reinterpret_cast<size_t>(dll);
	void *remoteFunction = reinterpret_cast<void*>(reinterpret_cast<size_t>(remoteDll) + offset);
	FreeLibrary(dll);

	// Call the function
	return CallRemoteFunction(process, remoteFunction, arg, argSize, result);
}

/// <summary>
/// Creates a process with a DLL injected into it.
/// The process will be created in a suspended state so that DLL calls can be made safely.
/// </summary>
/// <param name="dllPath">The path to the DLL to inject.</param>
/// <param name="dllHandle">If not NULL, pointer to a handle to store the remote handle of the DLL to.</param>
/// <returns><c>true</c> if successful.</returns>
BOOL WINAPI CreateProcessWithDLL(LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, const char *dllPath, HMODULE *dllHandle)
{
	// Create the process in a suspended state
	if (!CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation))
		return false;

	// Try to inject the DLL
	HMODULE handle = InjectDLLIntoProcess(lpProcessInformation->hProcess, dllPath);
	if (handle == NULL)
	{
		// Injection failed - kill the process
		TerminateProcess(lpProcessInformation->hProcess, 0);
		CloseHandle(lpProcessInformation->hThread);
		CloseHandle(lpProcessInformation->hProcess);
		return false;
	}
	if (dllHandle != NULL)
		*dllHandle = handle;
	return true;
}