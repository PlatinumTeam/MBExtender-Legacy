// PluginLoader.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "FuncInterceptor.h"
#include "BasicPluginInterface.h"
#include "SharedObject.h"
#include <TorqueLib/TGE.h>

#define PLUGIN_DIR "plugins/"
#define PLUGIN_FILTER (PLUGIN_DIR "*.dll")
#define PLUGIN_DIR_LEN 9

typedef void(*initMath_t)();
typedef void(*installOverrides_t)(TorqueFunctionInterceptor *interceptor);
typedef void(*initPlugin_t)(PluginInterface *plugin);

namespace
{
	CodeInjection::CodeAllocator* codeAlloc;
	CodeInjection::FuncInterceptor* hook;
	BasicTorqueFunctionInterceptor* basicInterceptor;

	SharedObject* mathLib;
	
	struct LoadedPlugin
	{
		boost::filesystem::path path;
		SharedObject *library;
		BasicPluginInterface *pluginInterface;
	};
	std::vector<LoadedPlugin>* loadedPlugins;

	void loadPlugins()
	{
		boost::filesystem::path pluginDir("plugins");
		if (!boost::filesystem::exists(pluginDir) || !boost::filesystem::is_directory(pluginDir))
		{
			TGE::Con::warnf("   No \"plugins\" directory found!");
			return;
		}
		
		std::vector<boost::filesystem::path> paths;
		std::copy(boost::filesystem::directory_iterator(pluginDir), boost::filesystem::directory_iterator(), std::back_inserter(paths));
		for (auto &path : paths)
		{
			if (path.extension() != SharedObject::DefaultExtension)
				continue;
			
			std::string pathStr = path.generic_string();
			TGE::Con::printf("   Loading %s", pathStr.c_str());
			SharedObject *library = new SharedObject(pathStr.c_str());
			if (library->loaded())
			{
				LoadedPlugin info = { path, library, new BasicPluginInterface(basicInterceptor, pathStr) };
				loadedPlugins->push_back(info);
			}
			else
			{
				delete library;
				TGE::Con::errorf("   Unable to load %s!", pathStr);
			}
		}
	}

	void callPluginInit(const char *message, const char *fnName)
	{
		if (loadedPlugins->size() == 0)
			return;
		TGE::Con::printf("%s", message);
		for (auto &plugin : *loadedPlugins)
		{
			const char *pathStr = plugin.path.generic_string().c_str();
			TGE::Con::printf("   Initializing %s", pathStr);

			// If it exports an initialization function, call it
			auto initFunc = reinterpret_cast<initPlugin_t>(plugin.library->getSymbol(fnName));
			if (initFunc)
				initFunc(plugin.pluginInterface);
			else
				TGE::Con::warnf("   WARNING: %s does not have a %s() function!", pathStr, fnName);
		}
		TGE::Con::printf("");
	}

	void pluginPreInit()
	{
		callPluginInit("MBExtender: Initializing Plugins, Stage 1:", "preEngineInit");
	}

	void pluginPostInit()
	{
		callPluginInit("MBExtender: Initializing Plugins, Stage 2:", "postEngineInit");
	}

	void setPluginVariables()
	{
		for (auto &plugin : *loadedPlugins)
		{
			// Set the Plugin::Loaded variable corresponding to the plugin
			std::string varName = plugin.path.stem().generic_string();
			varName = "Plugin::Loaded" + varName;
			TGE::Con::setBoolVariable(varName.c_str(), true);
		}
	}

	void loadMathLibrary()
	{
		TGE::Con::printf("   Initializing memory interface");
		std::string libPath = "./TorqueLib";
		libPath += SharedObject::DefaultExtension;
		mathLib = new SharedObject(libPath.c_str());
		if (mathLib->loaded())
		{
			auto initFunc = reinterpret_cast<initMath_t>(mathLib->getSymbol("init"));
			if (initFunc)
			{
				initFunc();
				return;
			}
		}
		TGE::Con::errorf("   Unable to load %s! Some plugins may fail to load!", libPath.c_str());
	}

	void installUserOverrides()
	{
		if (!mathLib || !mathLib->loaded())
			return;

		TGE::Con::printf("   Installing user overrides");
		auto installFunc = reinterpret_cast<installOverrides_t>(mathLib->getSymbol("installUserOverrides"));
		if (installFunc)
			installFunc(basicInterceptor);
		else
			TGE::Con::errorf("   TorqueLib is out-of-date and does not support user overrides!");
	}

	auto originalNsInit = TGE::Namespace::init;
	void newNsInit()
	{
		originalNsInit();

		TGE::Con::printf("MBExtender Init:");
		loadMathLibrary();
		loadPlugins();
		installUserOverrides();
		TGE::Con::printf("");
		pluginPreInit();
	}

	auto originalParticleInit = TGE::ParticleEngine::init;
	void newParticleInit()
	{
		originalParticleInit();

		pluginPostInit();
		setPluginVariables();
	}

	// Handles onClientProcess() callbacks
	auto originalClientProcess = TGE::clientProcess;
	void newClientProcess(U32 timeDelta)
	{
		BasicPluginInterface::executeProcessList(timeDelta);
		originalClientProcess(timeDelta);
	}
}

void installHooks()
{
	loadedPlugins = new std::vector<LoadedPlugin>();
	codeAlloc = new CodeInjection::CodeAllocator();
	hook = new CodeInjection::FuncInterceptor(codeAlloc);
	basicInterceptor = new BasicTorqueFunctionInterceptor(hook);
	
	// Intercept ParticleEngine::init() because it's the last module that loads before main.cs is executed
	originalNsInit = hook->intercept(TGE::Namespace::init, newNsInit);
	originalParticleInit = hook->intercept(TGE::ParticleEngine::init, newParticleInit);

	// Intercept clientProcess() to call plugin callbacks
	originalClientProcess = hook->intercept(TGE::clientProcess, newClientProcess);
}
