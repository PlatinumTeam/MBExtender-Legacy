// PluginLoader.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <string>
#include <TorqueLib/TGE.h>
#include "FuncInterceptor.h"
#include "BasicPluginInterface.h"
#include "SharedObject.h"
#include "Filesystem.h"
#include "StringUtil.h"

#if defined(_WIN32)
 #define PATH_PREFIX
 const char *const TorqueLibPath = PATH_PREFIX "TorqueLib.dll";
 #define MB_TEXT_START 0x401000
 #define MB_TEXT_SIZE  0x238000
#elif defined(__APPLE__)
 #define PATH_PREFIX "./Contents/MacOS/"
 const char *const TorqueLibPath = PATH_PREFIX "TorqueLib.dylib";
 #define MB_TEXT_START 0x2BC0
 #define MB_TEXT_SIZE  0x265FA6
#elif defined(__linux)
 #define PATH_PREFIX "./"
 const char *const TorqueLibPath = PATH_PREFIX "TorqueLib.so";
 #define MB_TEXT_START 0x804EBD0
 #define MB_TEXT_SIZE  0x277500
#endif

typedef void(*initMath_t)();
typedef void(*installOverrides_t)(PluginInterface *plugin);
typedef void(*pluginCallback_t)(PluginInterface *plugin);

namespace
{
	// These objects all have to be allocated manually in order to prevent initialization order issues
	CodeInjection::CodeAllocator *codeAlloc;
	CodeInjection::CodeInjectionStream *injectionStream;
	CodeInjection::FuncInterceptor *hook;

	SharedObject *mathLib;
	installOverrides_t installUserOverrides;
	
	struct LoadedPlugin
	{
		std::string path;
		SharedObject *library;
		CodeInjection::FuncInterceptor *interceptor;
		BasicTorqueFunctionInterceptor *torqueInterceptor;
		BasicPluginInterface *pluginInterface;
	};
	std::vector<LoadedPlugin> *loadedPlugins;

	void loadPlugins()
	{
		std::string pluginDir = PATH_PREFIX "plugins";
		if (!Filesystem::Directory::exists(pluginDir))
		{
			TGE::Con::warnf("   No %s directory found!", pluginDir.c_str());
			return;
		}
		std::vector<std::string> paths;
		if (!Filesystem::Directory::enumerate(pluginDir, &paths))
		{
			TGE::Con::warnf("   Unable to enumerate the %s directory!", pluginDir.c_str());
			return;
		}
		for (size_t i = 0; i < paths.size(); i++)
		{
			std::string &path = paths[i];

			// Check if the path points to a shared object file
			if (Filesystem::Path::getExtension(path) != SharedObject::DefaultExtension)
			{
				// Check if the path is a directory, and if so, try to load a shared object file inside it with the same name
				if (!Filesystem::Directory::exists(path))
					continue;
				std::string pluginName = Filesystem::Path::getFilename(path);
				path = Filesystem::Path::combine(path, pluginName + SharedObject::DefaultExtension);
				if (!Filesystem::File::exists(path))
					continue;
			}
			
			TGE::Con::printf("   Loading %s", path.c_str());
			SharedObject *library = new SharedObject(path.c_str());
			if (library->loaded())
			{
				CodeInjection::FuncInterceptor *interceptor = new CodeInjection::FuncInterceptor(injectionStream, codeAlloc);
				BasicTorqueFunctionInterceptor *torqueInterceptor = new BasicTorqueFunctionInterceptor(interceptor);
				BasicPluginInterface *pluginInterface = new BasicPluginInterface(torqueInterceptor, path);
				LoadedPlugin info = { path, library, interceptor, torqueInterceptor, pluginInterface };
				loadedPlugins->push_back(info);
				if (installUserOverrides)
					installUserOverrides(pluginInterface);
			}
			else
			{
				TGE::Con::errorf("   Unable to load %s!", path.c_str());
				delete library;
			}
		}
	}

	bool runPluginCallback(const LoadedPlugin *plugin, const char *fnName)
	{
		pluginCallback_t func = reinterpret_cast<pluginCallback_t>(plugin->library->getSymbol(fnName));
		if (!func)
			return false;
		func(plugin->pluginInterface);
		return true;
	}

	void callPluginInit(const char *message, const char *fnName)
	{
		if (loadedPlugins->size() == 0)
			return;
		TGE::Con::printf("%s", message);
		for (size_t i = 0; i < loadedPlugins->size(); i++)
		{
			LoadedPlugin &plugin = (*loadedPlugins)[i];
			TGE::Con::printf("   Initializing %s", plugin.path.c_str());
			if (!runPluginCallback(&plugin, fnName))
				TGE::Con::warnf("   WARNING: %s does not have a %s() function!", plugin.path.c_str(), fnName);
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

	void unloadPlugin(LoadedPlugin *plugin)
	{
		if (!runPluginCallback(plugin, "engineShutdown"))
			TGE::Con::warnf("   WARNING: %s does not have a %s() function!", plugin->path.c_str(), "engineShutdown");
		delete plugin->pluginInterface;
		delete plugin->torqueInterceptor;
		delete plugin->interceptor;
		delete plugin->library;
		plugin->pluginInterface = NULL;
		plugin->torqueInterceptor = NULL;
		plugin->interceptor = NULL;
		plugin->library = NULL;
	}

	void unloadPlugins()
	{
		TGE::Con::printf("MBExtender: Unloading Plugins:");
		for (size_t i = 0; i < loadedPlugins->size(); i++)
		{
			LoadedPlugin &plugin = (*loadedPlugins)[i];
			TGE::Con::printf("   Unloading %s", plugin.path.c_str());
			unloadPlugin(&plugin);
		}
		loadedPlugins->clear();
	}

	void setPluginVariables()
	{
		for (size_t i = 0; i < loadedPlugins->size(); i++)
		{
			LoadedPlugin &plugin = (*loadedPlugins)[i];

			// Set the Plugin::Loaded variable corresponding to the plugin
			std::string varName = Filesystem::Path::getFilenameWithoutExtension(plugin.path);
			varName = "Plugin::Loaded" + varName;
			TGE::Con::setBoolVariable(varName.c_str(), true);
		}
	}

	void loadMathLibrary()
	{
		TGE::Con::printf("   Initializing memory interface");
		mathLib = new SharedObject(TorqueLibPath);
		if (mathLib->loaded())
		{
			initMath_t initFunc = reinterpret_cast<initMath_t>(mathLib->getSymbol("init"));
			if (initFunc)
				initFunc();
			else
				TGE::Con::errorf("   TorqueLib does not have an init() function!");

			installUserOverrides = reinterpret_cast<installOverrides_t>(mathLib->getSymbol("installUserOverrides"));
			if (!installUserOverrides)
				TGE::Con::errorf("   TorqueLib does not support user overrides!");
		}
		else
		{
			TGE::Con::errorf("   Unable to load %s! Some plugins may fail to load!", TorqueLibPath);
			delete mathLib;
			mathLib = NULL;
		}
	}

	// TorqueScript function to unload a plugin given its name
	bool tsUnloadPlugin(TGE::SimObject *obj, S32 argc, const char *argv[])
	{
		std::string upperName = strToUpper(argv[1]);
		for (std::vector<LoadedPlugin>::iterator it = loadedPlugins->begin(); it != loadedPlugins->end(); ++it)
		{
			LoadedPlugin *plugin = &*it;
			if (strToUpper(Filesystem::Path::getFilenameWithoutExtension(plugin->path)) == upperName)
			{
				TGE::Con::printf("MBExtender: Unloading plugin %s", plugin->path.c_str());
				unloadPlugin(plugin);
				loadedPlugins->erase(it);
				return true;
			}
		}
		return false;
	}

	void registerFunctions()
	{
		TGE::Con::addCommand("unloadPlugin", tsUnloadPlugin, "unloadPlugin(name)", 2, 2);
	}

	void (*originalNsInit)() = TGE::Namespace::init;
	void newNsInit()
	{
		originalNsInit();

		TGE::Con::printf("MBExtender Init:");
		loadMathLibrary();
		loadPlugins();
		TGE::Con::printf("");
		pluginPreInit();
	}

	void (*originalParticleInit)() = TGE::ParticleEngine::init;
	void newParticleInit()
	{
		originalParticleInit();

		registerFunctions();
		pluginPostInit();
		setPluginVariables();
	}

	void (*originalShutdownGame)() = TGE::shutdownGame;
	void newShutdownGame()
	{
		originalShutdownGame();
		unloadPlugins();
	}

	// Handles onClientProcess() callbacks
	void (*originalClientProcess)(U32) = TGE::clientProcess;
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
	injectionStream = new CodeInjection::CodeInjectionStream(reinterpret_cast<void*>(MB_TEXT_START), MB_TEXT_SIZE);
	hook = new CodeInjection::FuncInterceptor(injectionStream, codeAlloc);
	
	// Intercept ParticleEngine::init() because it's the last module that loads before main.cs is executed
	originalNsInit = hook->intercept(TGE::Namespace::init, newNsInit);
	originalParticleInit = hook->intercept(TGE::ParticleEngine::init, newParticleInit);

	// Intercept shutdownGame() to unload plugins when the game exits
	originalShutdownGame = hook->intercept(TGE::shutdownGame, newShutdownGame);

	// Intercept clientProcess() to call plugin callbacks
	originalClientProcess = hook->intercept(TGE::clientProcess, newClientProcess);
}
