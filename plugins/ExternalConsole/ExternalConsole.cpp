// Demonstration of how to use the function interception mechanism to create an external console window for the game.

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <PluginLoader/PluginInterface.h>
#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
	void setAttributes(TGE::ConsoleLogEntry::Level level)
	{
#ifdef _WIN32
		WORD attributes;
		switch (level)
		{
		case TGE::ConsoleLogEntry::Warning:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case TGE::ConsoleLogEntry::Error:
			attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
		default:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
#endif
	}

	void resetAttributes()
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
	}

	std::mutex cmdMutex;
	std::string cmdString;

	void inputThread()
	{
		while (true)
		{
			std::string str;
			std::getline(std::cin, str);
			cmdMutex.lock();
			cmdString = str;
			cmdMutex.unlock();
		}
	}

	void onClientProcess(U32 timeDelta)
	{
		if (cmdMutex.try_lock()) // NOTE: This is a really bad way of synchronization, but it works
		{
			if (cmdString.length() > 0)
			{
				TGE::Con::evaluatef("%s", cmdString.c_str());
				cmdString = "";
			}
			cmdMutex.unlock();
		}
	}
}

TorqueOverride(void, Con::_printf, (TGE::ConsoleLogEntry::Level level, TGE::ConsoleLogEntry::Type type, const char *fmt, va_list argptr), originalPrintf)
{
	setAttributes(level);
	vprintf(fmt, argptr);
	putchar('\n');
	resetAttributes();
	originalPrintf(level, type, fmt, argptr);
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
#ifdef _WIN32
	// Set up a console window and redirect stdio to it
	AllocConsole();
	SetConsoleTitle("TGE Console");
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	std::ios::sync_with_stdio();
#endif
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	// Call our onClientProcess() function each tick
	plugin->onClientProcess(onClientProcess);
	
	// Create input-handling thread
	std::thread(inputThread).detach();
}
