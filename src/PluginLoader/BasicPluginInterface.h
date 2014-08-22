#ifndef PLUGINLOADER_BASICPLUGININTERFACE_H
#define PLUGINLOADER_BASICPLUGININTERFACE_H

#include <vector>
#include <string>
#include <PluginLoader/PluginInterface.h>
#include "FuncInterceptor.h"

class BasicTorqueFunctionInterceptor : public TorqueFunctionInterceptor
{
public:
	BasicTorqueFunctionInterceptor(CodeInjection::FuncInterceptor *interceptor);

	void restore(void *func);

protected:
	void* intercept(void *func, void *newFunc);

private:
	CodeInjection::FuncInterceptor *interceptor;
};

class BasicPluginInterface : public PluginInterface
{
public:
	BasicPluginInterface(BasicTorqueFunctionInterceptor *interceptor, const char *dllPath);

	const char* getPath();
	TorqueFunctionInterceptor* getInterceptor();
	void onClientProcess(clientProcess_ptr callback);

	static void executeProcessList(uint32_t timeDelta);

private:
	BasicTorqueFunctionInterceptor *interceptor;
	std::string path;
};

#endif // PLUGINLOADER_BASICPLUGININTERFACE_H
