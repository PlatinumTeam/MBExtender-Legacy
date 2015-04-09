#ifndef PLUGINLOADER_PLUGININTERFACE_H
#define PLUGINLOADER_PLUGININTERFACE_H

#include <cstdint>
#include <string>

// Use this macro with the preEngineInit() and postEngineInit() functions
// to ensure that they are exported from the plugin correctly
//
// e.g.
// PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
#ifdef _MSC_VER
	#define PLUGINCALLBACK extern "C" __declspec(dllexport)
#else
	//#if __GNUC__ >= 4
		#define PLUGINCALLBACK extern "C" __attribute__((visibility("default")))
	//#else
		//#define PLUGINCALLBACK
	//#endif
#endif

/// <summary>
/// Allows TGE functions to be intercepted.
/// </summary>
class TorqueFunctionInterceptor
{
public:
	virtual ~TorqueFunctionInterceptor() { }

	/// <summary>
	/// Intercepts a function so that all calls to it will redirect to another one.
	/// </summary>
	/// <param name="func">The function to intercept calls to.</param>
	/// <param name="newFunc">The function to redirect callers to.</param>
	/// <returns>A function pointer which can be used to call the original function, or <c>NULL</c> on failure.</returns>
	template<class T>
	T intercept(T func, T newFunc)
	{
		return reinterpret_cast<T>(intercept(reinterpret_cast<void*>(func), reinterpret_cast<void*>(newFunc)));
	}

	/// <summary>
	/// Restores a function that was intercepted.
	/// </summary>
	/// <param name="func">The function to restore.</param>
	virtual void restore(void *func) = 0;

protected:
	/// <summary>
	/// Intercepts a function so that all calls to it will redirect to another one.
	/// </summary>
	/// <param name="func">The function to intercept calls to.</param>
	/// <param name="newFunc">The function to redirect callers to.</param>
	/// <returns>A function pointer which can be used to call the original function, or <c>NULL</c> on failure.</returns>
	virtual void* intercept(void *func, void *newFunc) = 0;
};

/// <summary>
/// Interface which plugins can use to perform various actions in a platform-independent manner.
/// </summary>
class PluginInterface
{
public:
	virtual ~PluginInterface() { }

	/// <summary>
	/// Gets the path to the dynamic library for the current plugin.
	/// </summary>
	/// <returns>The path to the dynamic library for the current plugin. May not be absolute.</returns>
	virtual std::string getPath() const = 0;

	/// <summary>
	/// Gets an object which can be used to intercept Torque functions.
	/// </summary>
	/// <returns>An object which can be used to intercept Torque functions.</returns>
	virtual TorqueFunctionInterceptor *getInterceptor() const = 0;

	/// <summary>
	/// Function pointer type used by <see cref="onClientProcess"/> callbacks.
	/// </summary>
	typedef void (*clientProcess_ptr)(uint32_t delta);

	/// <summary>
	/// Registers a callback to be fired whenever clientProcess(U32) is called.
	/// Callbacks will be fired before anything else in the engine is updated.
	/// </summary>
	/// <param name="callback">The callback function to register.</param>
	virtual void onClientProcess(clientProcess_ptr callback) = 0;
};

#endif
