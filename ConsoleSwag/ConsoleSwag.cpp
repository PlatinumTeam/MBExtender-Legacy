// Demonstration of how to use the function interception mechanism to create an external console window for the game.

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <iostream>
#include "../PluginLoader/PluginInterface.h"

WORD getAttributes(TGE::ConsoleLogEntry::Level level)
{
	switch (level)
	{
	case TGE::ConsoleLogEntry::Warning:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	case TGE::ConsoleLogEntry::Error:
		return FOREGROUND_RED | FOREGROUND_INTENSITY;
	default:
		return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
}

TorqueOverride(void, Con::_printf, (TGE::ConsoleLogEntry::Level level, TGE::ConsoleLogEntry::Type type, const char *fmt, va_list argptr), originalPrintf)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), getAttributes(level));
	vprintf(fmt, argptr);
	putchar('\n');
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	originalPrintf(level, type, fmt, argptr);
}

std::mutex cmdMutex;
std::string cmdString;

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

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
	// Set up a console window and redirect stdio to it
	AllocConsole();
	SetConsoleTitle("TGE Console");
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	std::ios::sync_with_stdio();

	// Call our onClientProcess() function each tick
	plugin->onClientProcess(onClientProcess);
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	// Create input-handling thread
	std::thread(inputThread).detach();
}