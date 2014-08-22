#pragma once
#include <Windows.h>

/// <summary>
/// Injects a DLL into another process.
/// </summary>
/// <param name="process">A handle to the process to inject the DLL into.</param>
/// <param name="dllPath">The path of the DLL to inject.</param>
/// <returns>A handle to the DLL if successful, or <c>NULL</c> otherwise.</returns>
HMODULE InjectDLLIntoProcess(HANDLE process, const char *dllPath);

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
bool CallRemoteFunction(HANDLE process, const void *function, const void *arg, size_t argSize, DWORD *result);

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
bool CallRemoteDLLFunction(HANDLE process, HMODULE remoteDll, const char *dllPath, const char *funcName, const void *arg, const size_t argSize, DWORD *result);

/// <summary>
/// Creates a process with a DLL injected into it.
/// The process will be created in a suspended state so that DLL calls can be made safely.
/// </summary>
/// <param name="dllPath">The path to the DLL to inject.</param>
/// <param name="dllHandle">If not NULL, pointer to a handle to store the remote handle of the DLL to.</param>
/// <returns><c>true</c> if successful.</returns>
BOOL WINAPI CreateProcessWithDLL(LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation, const char *dllPath, HMODULE *dllHandle);