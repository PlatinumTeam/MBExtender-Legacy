#pragma once

#include <intrin.h>
#include <Windows.h>
#include "math/mMath.h"
#include "QuickOverride.h"
#include "../PluginLoader/PluginInterface.h"

// Hooray for super hacked-together CPU detection code!
#define CPUID_INDEX_SSE 3
#define CPUID_FLAG_SSE (1 << 25)

#define CPUID_INDEX_3DNOW 3
#define CPUID_FLAG_3DNOW (1 << 31)
#define CPUID_INDEX_3DNOW_PREFETCH 2
#define CPUID_FLAG_3DNOW_PREFETCH (1 << 8)

struct AvailableExtensions
{
	bool supports3DNow;
	bool supportsSSE;
};

AvailableExtensions detectExtensions()
{
	AvailableExtensions result;
	int cpuInfo[4];

	// Detect SSE support
	__cpuid(cpuInfo, 1);
	result.supportsSSE = ((cpuInfo[CPUID_INDEX_SSE] & CPUID_FLAG_SSE) != 0);
	
	// Detect 3DNow support
	// Support for the prefetch instruction is also necessary
	__cpuid(cpuInfo, 0x80000001);
	bool has3dnow = ((cpuInfo[CPUID_INDEX_3DNOW] & CPUID_FLAG_3DNOW) != 0);
	bool hasPrefetch = ((cpuInfo[CPUID_INDEX_3DNOW_PREFETCH] & CPUID_FLAG_3DNOW_PREFETCH) != 0);
	result.supports3DNow = (has3dnow && hasPrefetch);

	return result;
}

void mInstallLibrary_C();
//void mInstallLibrary_ASM();
void mInstall_AMD_Math();
void mInstall_Library_SSE();

void installExtensions()
{
	mInstallLibrary_C();
	//mInstallLibrary_ASM(); // TODO: Add support for the assembly routines

	AvailableExtensions exts = detectExtensions();
	if (exts.supports3DNow)
		mInstall_AMD_Math();
	if (exts.supportsSSE)
		mInstall_Library_SSE();
}

// TODO: Export this and hook the setRandomSeed console function
void seedGenerator()
{
	MRandomLCG::setGlobalRandSeed(GetTickCount());
}

namespace TorqueLib
{
	__declspec(dllexport) OverrideRequest *requestList = nullptr;
}

extern "C" __declspec(dllexport) void init()
{
	installExtensions();
	seedGenerator();
}

extern "C" __declspec(dllexport) void installUserOverrides(TorqueFunctionInterceptor *interceptor)
{
	TorqueLib::OverrideRequest *currentOverride = TorqueLib::requestList;
	while (currentOverride)
	{
		typedef void (*tmpfnptr_t)(); // The two pointers need to be casted to something, since intercept doesn't allow two void*'s for safety reasons
		tmpfnptr_t originalFn = static_cast<tmpfnptr_t>(*currentOverride->originalFunctionPtr);
		tmpfnptr_t newFn = static_cast<tmpfnptr_t>(currentOverride->newFunction);
		*currentOverride->originalFunctionPtr = interceptor->intercept(originalFn, newFn);
		currentOverride = currentOverride->nextOverride;
	}
}