#include <TorqueLib/math/mMath.h>
#include <TorqueLib/QuickOverride.h>
#include <PluginLoader/PluginInterface.h>
#include <ctime>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_CPUID_H
#include <cpuid.h>
#endif
#ifdef HAVE_INTRIN_H
#include <intrin.h>
#endif

// Hooray for super hacked-together CPU detection code!
#define CPUID_FLAG_SSE (1 << 25)
#define CPUID_FLAG_3DNOW (1 << 31)
#define CPUID_FLAG_3DNOW_PREFETCH (1 << 8)

struct AvailableExtensions
{
	bool supports3DNow;
	bool supportsSSE;
};

AvailableExtensions detectExtensions()
{
	AvailableExtensions result;
	unsigned int eax1, ebx1, ecx1, edx1;
	unsigned int eax1_ex, ebx1_ex, ecx1_ex, edx1_ex;
	
#ifdef HAVE_CPUID_H
	__get_cpuid(1, &eax1, &ebx1, &ecx1, &edx1);
	__get_cpuid(0x80000001, &eax1_ex, &ebx1_ex, &ecx1_ex, &edx1_ex);
#endif
#ifdef HAVE_INTRIN_H
	int cpuInfo1[4];
	int cpuInfo1_ex[4];
	__cpuid(cpuInfo1, 1);
	__cpuid(cpuInfo1_ex, 0x80000001);
	eax1 = cpuInfo1[0]; ebx1 = cpuInfo1[1]; ecx1 = cpuInfo1[2]; edx1 = cpuInfo1[3];
	eax1_ex = cpuInfo1_ex[0]; ebx1_ex = cpuInfo1_ex[1]; ecx1_ex = cpuInfo1_ex[2]; edx1_ex = cpuInfo1_ex[3];
#endif

	// Detect SSE support
	result.supportsSSE = ((edx1 & CPUID_FLAG_SSE) != 0);
	
	// Detect 3DNow support
	// Support for the prefetch instruction is also necessary
	bool has3dnow = ((edx1_ex & CPUID_FLAG_3DNOW) != 0);
	bool hasPrefetch = ((ecx1_ex & CPUID_FLAG_3DNOW_PREFETCH) != 0);
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
	{
		TGE::Con::printf("      3DNow detected");
		mInstall_AMD_Math();
	}
	if (exts.supportsSSE)
	{
		TGE::Con::printf("      SSE detected");
		mInstall_Library_SSE();
	}
}

// TODO: Export this and hook the setRandomSeed console function
void seedGenerator()
{
	MRandomLCG::setGlobalRandSeed(static_cast<U32>(time(NULL)));
}

namespace TorqueLib
{
	DLLSPEC OverrideRequest *requestList = NULL;
}

extern "C" DLLSPEC void init()
{
	installExtensions();
	seedGenerator();
}

extern "C" DLLSPEC void installUserOverrides(PluginInterface *plugin)
{
	TorqueFunctionInterceptor *interceptor = plugin->getInterceptor();
	TorqueLib::OverrideRequest *currentOverride = TorqueLib::requestList;
	while (currentOverride)
	{
		typedef void (*tmpfnptr_t)(); // The two pointers need to be casted to something, since intercept doesn't allow two void*'s for safety reasons
		tmpfnptr_t originalFn = reinterpret_cast<tmpfnptr_t>(*currentOverride->originalFunctionPtr);
		tmpfnptr_t newFn = reinterpret_cast<tmpfnptr_t>(currentOverride->newFunction);
		*currentOverride->originalFunctionPtr = reinterpret_cast<void*>(interceptor->intercept(originalFn, newFn));
		currentOverride = currentOverride->nextOverride;
	}
	TorqueLib::requestList = NULL;
}
