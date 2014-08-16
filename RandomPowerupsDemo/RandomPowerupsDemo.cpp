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

TorqueOverrideMember(void, Marble::doPowerUp, (TGE::Marble *thisObj, int id), originalDoPowerUp)
{
	int newId = availablePowerUps[gRandGen.randI(0, 2)];
	TGE::Con::printf("Forcing random powerup: %s", powerUpNames[newId - 1]);
	originalDoPowerUp(thisObj, newId);
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
}