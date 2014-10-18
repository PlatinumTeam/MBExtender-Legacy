#include <cstdio>

void installHooks();

__attribute__((constructor))
void initPluginLoader()
{
	// TODO: Verify that we're actually loaded into the correct version of the game
	printf("PluginLoader: installing hooks\n");
	installHooks();
	printf("PluginLoader: hooks installed\n");
}
