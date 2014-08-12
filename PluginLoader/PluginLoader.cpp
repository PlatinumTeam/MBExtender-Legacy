// PluginLoader.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <string>
#include "TGE.h"
#include "FuncInterceptor.h"
#include "Win32PluginInterface.h"

#define PLUGIN_DIR "plugins/"
#define PLUGIN_FILTER (PLUGIN_DIR "*.dll")
#define PLUGIN_DIR_LEN 9

static CodeInjection::CodeAllocator codeAlloc;
static CodeInjection::FuncInterceptor hook(&codeAlloc);
static Win32TorqueFunctionInterceptor win32Interceptor(&hook);

typedef void (*initMath_t)();
typedef void (*initPlugin_t)(PluginInterface *plugin);

struct LoadedPlugin
{
	std::string path;
	HMODULE module;
};
static std::vector<LoadedPlugin> loadedPlugins;

static void loadPlugins()
{
	// Scan for plugin files and load each one of them
	WIN32_FIND_DATA fileInfo;
	HANDLE find = FindFirstFile(PLUGIN_FILTER, &fileInfo);
	char pathBuf[PLUGIN_DIR_LEN + MAX_PATH];
	while (find != INVALID_HANDLE_VALUE)
	{
		// Prepend the plugin dir to the filename and print a debug message
		sprintf_s(pathBuf, PLUGIN_DIR "%s", fileInfo.cFileName);
		TGE::Con::printf("   Loading %s", pathBuf);

		// Attempt to load the DLL
		HMODULE plugin = LoadLibrary(pathBuf);
		if (plugin != NULL)
		{
			LoadedPlugin info = { pathBuf, plugin };
			loadedPlugins.push_back(info);
		}
		else
		{
			TGE::Con::errorf("   Unable to load %s!", pathBuf);
		}
		
		// Move to the next file
		if (!FindNextFile(find, &fileInfo))
		{
			FindClose(find);
			find = INVALID_HANDLE_VALUE;
		}
	}
}

static void callPluginInit(const char *fnName)
{
	for (auto &plugin : loadedPlugins)
	{
		TGE::Con::printf("   Initializing %s", plugin.path.c_str());

		// If it exports an initialization function, call it
		auto initFunc = reinterpret_cast<initPlugin_t>(GetProcAddress(plugin.module, fnName));
		if (initFunc != NULL)
			initFunc(new Win32PluginInterface(&win32Interceptor, plugin.path.c_str()));
		else
			TGE::Con::warnf("   WARNING: %s does not have a %s() function!", fnName, plugin.path);
	}
}

static void pluginPreInit()
{
	if (loadedPlugins.size() == 0)
		return;

	TGE::Con::printf("MBExtender: Initializing Plugins, Stage 1:");
	callPluginInit("preEngineInit");
	TGE::Con::printf("");
}

static void pluginPostInit()
{
	if (loadedPlugins.size() == 0)
		return;

	TGE::Con::printf("MBExtender: Initializing Plugins, Stage 2:");
	callPluginInit("postEngineInit");
	TGE::Con::printf("");
}

static void setPluginVariables()
{
	for (auto &plugin : loadedPlugins)
	{
		// Set the Plugin::Loaded variable corresponding to the plugin
		std::string varName = plugin.path;
		varName.erase(varName.find_last_of('.')); // Remove file extension
		varName = "Plugin::Loaded" + varName;
		TGE::Con::setBoolVariable(varName.c_str(), true);
	}
}

static void loadMathLibrary()
{
	TGE::Con::printf("   Initializing plugin math library");
	HMODULE mathLib = LoadLibrary("TorqueMath.dll");
	if (mathLib != NULL)
	{
		auto initFunc = reinterpret_cast<initMath_t>(GetProcAddress(mathLib, "init"));
		if (initFunc != NULL)
		{
			initFunc();
			return;
		}
	}
	TGE::Con::errorf("   Unable to load TorqueMath.dll! Some plugins may fail to load!");
}

static auto originalNsInit = TGE::Namespace::init;
static void newNsInit()
{
	originalNsInit();

	TGE::Con::printf("MBExtender Init:");
	loadMathLibrary();
	loadPlugins();
	TGE::Con::printf("");
	pluginPreInit();
}

static auto originalParticleInit = TGE::ParticleEngine::init;
static void newParticleInit()
{
	originalParticleInit();

	pluginPostInit();
	setPluginVariables();
}

// Handles onClientProcess() callbacks
static auto originalClientProcess = TGE::clientProcess;
static void newClientProcess(U32 timeDelta)
{
	Win32PluginInterface::executeProcessList(timeDelta);
	originalClientProcess(timeDelta);
}

#ifdef DISABLE_NETWORK
static bool myNetInit()
{
	return true;
}
#endif

static bool verifyGame()
{
	const char *testPointer = reinterpret_cast<const char*>(0x6796C4);
	const char *testStr = "Marble Blast";

	// Make sure we can actually read from the test memory location
	MEMORY_BASIC_INFORMATION memInfo;
	if (VirtualQuery(testPointer, &memInfo, sizeof(memInfo)) != sizeof(memInfo))
		return false;
	if (memInfo.Protect == 0 || (memInfo.Protect & PAGE_NOACCESS) || (memInfo.Protect & PAGE_EXECUTE))
		return false;

	// Check if the string matches
	if (memcmp(testPointer, testStr, strlen(testStr)) != 0)
		return false;

/*#ifdef NO_MBP
	// This is "platinumbeta" XOR encrypted with "PQ" as the key
	// We decrypt it and then check if the folder exists to see if the game is MBP or not
	// Pretty basic, but it's not worth it to do anything more complicated
	const char *mbpFolderEncrypted = " =1%9?%<24$0";
	const size_t mbpFolderLength = 12;
	char mbpFolderDecrypted[mbpFolderLength + 1];
	for (int i = 0; i < mbpFolderLength; i += 2)
	{
		mbpFolderDecrypted[i] = mbpFolderEncrypted[i] ^ 'P';
		mbpFolderDecrypted[i + 1] = mbpFolderEncrypted[i + 1] ^ 'Q';
	}
	mbpFolderDecrypted[mbpFolderLength] = '\0';

	// Check if the folder exists
	DWORD attributes = GetFileAttributes(mbpFolderDecrypted);
	if (attributes != INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY)
		return false;
#endif*/

	return true;
}

DWORD WINAPI initPluginLoader(LPVOID unused)
{
	if (!verifyGame())
	{
		MessageBox(NULL, "MBExtender is only compatible with the full version of Marble Blast Gold.", "MBExtender", MB_OK | MB_ICONERROR);
		return false;
	}

	// Intercept ParticleEngine::init() because it's the last module that loads before main.cs is executed
	originalNsInit = hook.intercept(TGE::Namespace::init, newNsInit);
	originalParticleInit = hook.intercept(TGE::ParticleEngine::init, newParticleInit);

	// Intercept clientProcess() to call plugin callbacks
	originalClientProcess = hook.intercept(TGE::clientProcess, newClientProcess);

#ifdef DISABLE_NETWORK
	hook.intercept(TGE::Net::init, myNetInit);
#endif

	return true;
}