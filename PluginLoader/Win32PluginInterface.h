#pragma once

#include <vector>
#include "PluginInterface.h"
#include "FuncInterceptor.h"

class Win32TorqueFunctionInterceptor : public TorqueFunctionInterceptor
{
public:
	Win32TorqueFunctionInterceptor(CodeInjection::FuncInterceptor *interceptor);

	void restore(void *func);

protected:
	void* intercept(void *func, void *newFunc);

private:
	CodeInjection::FuncInterceptor *interceptor;
};

class Win32PluginInterface : public PluginInterface
{
public:
	Win32PluginInterface(Win32TorqueFunctionInterceptor *interceptor, const char *dllPath);

	const char* getPath();
	TorqueFunctionInterceptor* getInterceptor();
	void onClientProcess(clientProcess_t callback);

	static void executeProcessList(U32 timeDelta);

private:
	Win32TorqueFunctionInterceptor *interceptor;
	char *path;
	
	static std::vector<clientProcess_t> processList;
};