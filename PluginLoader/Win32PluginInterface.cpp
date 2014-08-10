#include <vector>
#include <cstdlib>
#include <cstring>
#include "Win32PluginInterface.h"

std::vector<PluginInterface::clientProcess_t> Win32PluginInterface::processList;

Win32TorqueFunctionInterceptor::Win32TorqueFunctionInterceptor(CodeInjection::FuncInterceptor *interceptor)
	: interceptor(interceptor)
{
}

void Win32TorqueFunctionInterceptor::restore(void *func)
{
	interceptor->restore(func);
}

void* Win32TorqueFunctionInterceptor::intercept(void *func, void *newFunc)
{
	return interceptor->intercept(func, newFunc);
}

Win32PluginInterface::Win32PluginInterface(Win32TorqueFunctionInterceptor *interceptor, const char *dllPath)
	: interceptor(interceptor)
{
	size_t pathLen = strlen(dllPath);
	path = new char[pathLen];
	memcpy(path, dllPath, pathLen + 1);
}

/// <summary>
/// Gets the path.
/// </summary>
/// <returns></returns>
const char* Win32PluginInterface::getPath()
{
	return path;
}

TorqueFunctionInterceptor* Win32PluginInterface::getInterceptor()
{
	return interceptor;
}

void Win32PluginInterface::onClientProcess(clientProcess_t callback)
{
	processList.push_back(callback);
}

void Win32PluginInterface::executeProcessList(U32 timeDelta)
{
	auto size = processList.size();
	for (decltype(size) i = 0; i < size; i++)
		processList[i](timeDelta);
}