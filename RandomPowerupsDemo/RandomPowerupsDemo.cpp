// Intercepts Marble::doPowerUp() so that it always executes a random powerup :F

#include "../PluginLoader/PluginInterface.h"

int availablePowerUps[3] =
{
	1, // Super Jump
	2, // Super Speed
	5  // Gyrocopter
};

const char *powerUpNames[5] =
{
	"Super Jump",
	"Super Speed",
	"Super Bounce",
	"Shock Absorber",
	"Gyrocopter"
};

static auto originalDoPowerUp = TGE::Members::Marble::doPowerUp;
THISFN(void, myDoPowerUp, (TGE::Marble *thisObj, int id))
{
	int newId = availablePowerUps[gRandGen.randI(0, 2)];
	TGE::Con::printf("Forcing random powerup: %s", powerUpNames[newId - 1]);
	originalDoPowerUp(thisObj, newId);
}

PLUGINAPI void initPlugin(PluginInterface *plugin)
{
	auto interceptor = plugin->getInterceptor();
	originalDoPowerUp = interceptor->intercept(TGE::Members::Marble::doPowerUp, myDoPowerUp);
}