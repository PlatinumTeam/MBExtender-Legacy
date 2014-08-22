// Fakes a valid ignition ticket on the Linux version of the game.

#ifndef __linux
#error FakeIgnition is specific to the Linux version of the game.
#endif

#include <PluginLoader/PluginInterface.h>
#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>

namespace TGE
{
	namespace Auth
	{
		enum IgnitionStatus
		{
			Success,
			NotOpen,
			ArgumentError,
			FileError,
			ValidTicket,
			InvalidTicket,
			ExpiredTicket,
			InvalidMachine,
			InvalidDate,
			Wait,
			InvalidKey,
			DisableKey,
			ConnectionError,
			ServerError,
			Cancelled
		};
	
		FN(IgnitionStatus, validateIgnition, (), 0x821E184);
	}
}

TorqueOverride(TGE::Auth::IgnitionStatus, Auth::validateIgnition, (), oldValidateIgnition)
{
	// Make the game think that the ignition file has already been validated
	TGE::Con::printf("FakeIgnition: forcing ignition validation status to ValidTicket");
	return TGE::Auth::ValidTicket;
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
}
