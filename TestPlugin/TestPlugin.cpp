// Simple plugin for testing stuff

/*#include <math/mMath.h>
#include <math/mathUtils.h>*/
#include "../PluginLoader/PluginInterface.h"

ConsoleFunction(pq, void, 1, 1, "pq()")
{
	TGE::Con::printf("PQ WHERe");
}

/*ConsoleFunction(testCam, void, 2, 2, "testCam(marble)")
{
	TGE::Marble *marble = static_cast<TGE::Marble*>(TGE::Sim::findObject(argv[1]));
	if (marble != nullptr)
	{
		MatrixF mat;
		marble->getCameraTransform(nullptr, &mat);
		for (int i = 0; i < 4; i++)
			TGE::Con::printf("%f %f %f %f", mat(i, 0), mat(i, 1), mat(i, 2), mat(i, 3));
	}
	else
	{
		TGE::Con::errorf("marble == nullptr!");
	}
}*/

PLUGINAPI void initPlugin(PluginInterface *plugin)
{
	/*// This might seem useless, but it forces the plugin to import from TorqueMath.dll
	// So it lets us test TorqueMath.dll loading
	MathUtils::randomPointInSphere(10);

	TGE::Con::printf("      Hello from %s!", plugin->getPath());*/
}