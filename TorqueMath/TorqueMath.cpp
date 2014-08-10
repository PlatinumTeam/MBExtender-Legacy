#pragma once

#include <intrin.h>
#include <Windows.h>
#include "math/mMath.h"

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

extern "C" __declspec(dllexport) void init()
{
	installExtensions();
	seedGenerator();
}