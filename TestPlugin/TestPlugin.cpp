// Simple plugin for testing stuff

#include <cstdio>
#include <math/mMath.h>
#include <math/mathUtils.h>
#include "../PluginLoader/PluginInterface.h"

ConsoleFunction(pq, void, 1, 1, "pq()")
{
	TGE::Con::printf("PQ WHERe");
}

ConsoleFunction(testCam, void, 2, 2, "testCam(marble)")
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
}

ConsoleFunction(idTest, void, 2, 2, "idTest(obj)")
{
	TGE::SimObject *obj = static_cast<TGE::SimObject*>(TGE::Sim::findObject(argv[1]));
	TGE::Con::printf("getId() -> %d", obj->getId());
	TGE::Con::printf("getIdString() -> \"%s\"", obj->getIdString());
}

bool initializingClasses = false;

TorqueOverride(void, Members::AbstractClassRep::initialize, (), originalInitialize)
{
	TGE::Con::printf("ACR init");

	initializingClasses = true;
	originalInitialize();
	initializingClasses = false;
}

TorqueOverride(void, dQsort, (void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void*, const void*)), originalDQSort)
{
	originalDQSort(base, nelem, width, fcmp);
	if (initializingClasses)
	{
		TGE::AbstractClassRep **classes = static_cast<TGE::AbstractClassRep**>(base);
		for (U32 i = 0; i < nelem; i++)
		{
			TGE::Con::printf("Class %s -> ID %d", classes[i]->getClassName(), i);
		}
		TGE::Con::printf(" ");
	}
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	// This might seem useless, but it forces the plugin to import from TorqueLib.dll
	// So it lets us test TorqueLib.dll loading
	MathUtils::randomPointInSphere(10);

	TGE::Con::printf("      Hello from %s!", plugin->getPath());
}