#include <iostream>

void installHooks();

__attribute__((constructor))
void initPluginLoader()
{
	// TODO: Verify that we're actually loaded into the correct version of the game
	std::cout << "PluginLoader: installing hooks" << std::endl;
	installHooks();
	std::cout << "PluginLoader: hooks installed" << std::endl;
}
