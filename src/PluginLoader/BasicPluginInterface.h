#ifndef PLUGINLOADER_BASICPLUGININTERFACE_H
#define PLUGINLOADER_BASICPLUGININTERFACE_H

#include <vector>
#include <string>
#include <PluginLoader/PluginInterface.h>
#include "FuncInterceptor.h"

class BasicTorqueFunctionInterceptor : public TorqueFunctionInterceptor
{
public:
	BasicTorqueFunctionInterceptor(CodeInjection::FuncInterceptor *interceptor)
		: interceptor(interceptor)
	{
	}

	void restore(void *func);

protected:
	void* intercept(void *func, void *newFunc);

private:
	CodeInjection::FuncInterceptor *interceptor;
};

class BasicPluginInterface : public PluginInterface
{
public:
	BasicPluginInterface(TorqueFunctionInterceptor *interceptor, const std::string &dllPath)
		: interceptor(interceptor), path(dllPath)
	{
	}

	std::string getPath() const { return path; }
	TorqueFunctionInterceptor *getInterceptor() const { return interceptor; }
	void onClientProcess(clientProcess_ptr callback);

	static void executeProcessList(uint32_t timeDelta);

private:
	TorqueFunctionInterceptor *interceptor;
	std::string path;
};

#endif // PLUGINLOADER_BASICPLUGININTERFACE_H
