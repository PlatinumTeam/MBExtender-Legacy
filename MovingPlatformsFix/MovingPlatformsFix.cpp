// Fixes moving platforms in multiplayer

#include "../PluginLoader/PluginInterface.h"

static bool gLocalUpdate = false;

// Hook for Marble::advancePhysics that sets gLocalUpdate to true if a local update is occurring
auto originalAdvancePhysics = TGE::Members::Marble::advancePhysics;
THISFN(void, myAdvancePhysics, (TGE::Marble *thisObj, const TGE::Move *move, U32 delta))
{
	if (TGE::NetConnection::getConnectionToServer() == thisObj->getControllingClient())
		gLocalUpdate = true;

	originalAdvancePhysics(thisObj, move, delta);

	gLocalUpdate = false;
}

// Hook for PathedInterior::computeNextPathStep that only lets the call succeed if a local update is occurring
auto originalComputeNextPathStep = TGE::Members::PathedInterior::computeNextPathStep;
THISFN(void, myComputeNextPathStep, (TGE::PathedInterior *thisObj, U32 delta))
{
	if (gLocalUpdate)
		originalComputeNextPathStep(thisObj, delta);
}

// Hook for PathedInterior::advance that only lets the call succeed if a local update is occurring
auto originalAdvance = TGE::Members::PathedInterior::advance;
THISFN(void, myAdvance, (TGE::PathedInterior *thisObj, double delta))
{
	if (gLocalUpdate)
		originalAdvance(thisObj, delta);
}

PLUGINAPI void preEngineInit(PluginInterface *plugin)
{
}

PLUGINAPI void postEngineInit(PluginInterface *plugin)
{
	auto interceptor = plugin->getInterceptor();
	originalAdvancePhysics = interceptor->intercept(originalAdvancePhysics, myAdvancePhysics);
	originalComputeNextPathStep = interceptor->intercept(originalComputeNextPathStep, myComputeNextPathStep);
	originalAdvance = interceptor->intercept(originalAdvance, myAdvance);
}