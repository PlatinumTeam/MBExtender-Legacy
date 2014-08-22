#include <cstdlib>
#include <cstring>
#include <vector>
#include "BasicPluginInterface.h"

std::vector<PluginInterface::clientProcess_ptr>* processList;

BasicTorqueFunctionInterceptor::BasicTorqueFunctionInterceptor(CodeInjection::FuncInterceptor *interceptor)
	: interceptor(interceptor)
{
}

void BasicTorqueFunctionInterceptor::restore(void *func)
{
	interceptor->restore(func);
}

void* BasicTorqueFunctionInterceptor::intercept(void *func, void *newFunc)
{
	return interceptor->intercept(func, newFunc);
}

BasicPluginInterface::BasicPluginInterface(BasicTorqueFunctionInterceptor *interceptor, const char *dllPath)
	: interceptor(interceptor), path(dllPath)
{
}

/// <summary>
/// Gets the path.
/// </summary>
/// <returns></returns>
const char* BasicPluginInterface::getPath()
{
	return path.c_str();
}

TorqueFunctionInterceptor* BasicPluginInterface::getInterceptor()
{
	return interceptor;
}

void BasicPluginInterface::onClientProcess(clientProcess_ptr callback)
{
	if (!processList)
		processList = new std::vector<clientProcess_ptr>();
	processList->push_back(callback);
}

void BasicPluginInterface::executeProcessList(uint32_t timeDelta)
{
	if (!processList)
		return;
	auto size = processList->size();
	for (decltype(size) i = 0; i < size; i++)
		(*processList)[i](timeDelta);
}
