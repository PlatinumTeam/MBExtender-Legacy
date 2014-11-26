/* Mac and Linux only: makes the game's network protocol compatible with the one
 * used by the Windows engine
 * 
 * Without this plugin enabled, if someone playing on the Mac or Linux build of
 * the game attempts to connect to a player using the Windows build of the game,
 * their game will crash due to differences in how class IDs are assigned.
 * 
 * By default, the game engine assigns class IDs by sorting networked classes
 * in alphabetical order by name and then giving each class an ID corresponding
 * to its position in the sorted list. The issue with this is that the Mac and
 * Linux engines have Splash and SplashData classes which are not present in the
 * Windows version of the game, causing the class IDs to be misaligned.
 * 
 * This plugin forces the IDs of classes common to all platforms to match by
 * moving the Splash and SplashData classes to the end of the list after the
 * classes have been sorted. By hijacking dQsort() calls made by
 * AbstractClassRep::initialize(), we have full control over how class IDs are
 * assigned.
 * 
 * Some versions of the engine also have fxLight and fxLightData classes, but it
 * does not matter because the class names start with lowercase letters and will
 * always end up at the end of the list.
 */

#include <cstring>
#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>
#include <PluginLoader/PluginInterface.h>

// Set to true if AbstractClassRep::initialize() is running
bool initializingClasses = false;

TorqueOverride(void, Members::AbstractClassRep::initialize, (), originalInitialize)
{
	initializingClasses = true;
	originalInitialize();
	initializingClasses = false;
}

TorqueOverride(void, dQsort, (void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void*, const void*)), originalDQSort)
{
	originalDQSort(base, nelem, width, fcmp);
	if (initializingClasses)
	{
		// Search the class list for Splash and SplashData, and move them to the end
		// Note that dQsort is called once for each group of classes and that
		// Splash and SplashData are in separate groups, so once either is found
		// it's safe to break out of the loop as an optimization
		TGE::AbstractClassRep **classes = static_cast<TGE::AbstractClassRep**>(base);
		for (U32 i = 0; i < nelem; i++)
		{
			TGE::AbstractClassRep *current = classes[i];
			const char *name = current->getClassName();
			if (strcmp(name, "Splash") == 0 || strcmp(name, "SplashData") == 0)
			{
				TGE::Con::printf("Reallocating class ID for %s", name);
				memmove(&classes[i], &classes[i + 1], (nelem - i - 1) * sizeof(TGE::AbstractClassRep*));
				classes[nelem - 1] = current;
				break;
			}
		}
	}
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void engineShutdown(PluginInterface *plugin)
{
}