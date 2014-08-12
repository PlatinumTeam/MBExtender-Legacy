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

auto originalInitialize = TGE::Members::AbstractClassRep::initialize;
void myInitialize()
{
	initializingClasses = true;
	originalInitialize();
	initializingClasses = false;
}

auto originalDQSort = TGE::dQsort;
void myDQSort(void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void*, const void*))
{
	originalDQSort(base, nelem, width, fcmp);
	if (initializingClasses)
	{
		TGE::AbstractClassRep **classes = static_cast<TGE::AbstractClassRep**>(base);
		for (U32 i = 0; i < nelem; i++)
		{
			TGE::Con::printf("Class %s -> ID %d", classes[i]->getClassName(), i);
		}
		TGE::Con::printf("");
	}
}

PLUGINAPI void preEngineInit(PluginInterface *plugin)
{
	auto interceptor = plugin->getInterceptor();
	originalInitialize = interceptor->intercept(originalInitialize, myInitialize);
	originalDQSort = interceptor->intercept(originalDQSort, myDQSort);
}

PLUGINAPI void postEngineInit(PluginInterface *plugin)
{
	// This might seem useless, but it forces the plugin to import from TorqueMath.dll
	// So it lets us test TorqueMath.dll loading
	MathUtils::randomPointInSphere(10);

	TGE::Con::printf("      Hello from %s!", plugin->getPath());
}