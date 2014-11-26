// Fixes moving platforms in multiplayer

#include <PluginLoader/PluginInterface.h>
#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>

static bool gLocalUpdate = false;

// Hook for Marble::advancePhysics that sets gLocalUpdate to true if a local update is occurring
TorqueOverrideMember(void, Marble::advancePhysics, (TGE::Marble *thisObj, const TGE::Move *move, U32 delta), originalAdvancePhysics)
{
	if (TGE::NetConnection::getConnectionToServer() == thisObj->getControllingClient())
		gLocalUpdate = true;

	originalAdvancePhysics(thisObj, move, delta);

	gLocalUpdate = false;
}

// Hook for PathedInterior::computeNextPathStep that only lets the call succeed if a local update is occurring
TorqueOverrideMember(void, PathedInterior::computeNextPathStep, (TGE::PathedInterior *thisObj, U32 delta), originalComputeNextPathStep)
{
	if (gLocalUpdate)
		originalComputeNextPathStep(thisObj, delta);
}

// Hook for PathedInterior::advance that only lets the call succeed if a local update is occurring
TorqueOverrideMember(void, PathedInterior::advance, (TGE::PathedInterior *thisObj, double delta), originalAdvance)
{
	if (gLocalUpdate)
		originalAdvance(thisObj, delta);
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