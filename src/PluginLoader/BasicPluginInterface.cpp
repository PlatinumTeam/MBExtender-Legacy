#include <cstdlib>
#include <cstring>
#include <vector>
#include "BasicPluginInterface.h"

std::vector<PluginInterface::clientProcess_ptr> *processList;

void BasicTorqueFunctionInterceptor::restore(void *func)
{
	interceptor->restore(func);
}

void* BasicTorqueFunctionInterceptor::intercept(void *func, void *newFunc)
{
	return interceptor->intercept(func, newFunc);
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
	size_t size = processList->size();
	for (size_t i = 0; i < size; i++)
		(*processList)[i](timeDelta);
}
