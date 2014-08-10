// TGE.h: Interface to core TGE functions.
// Usable both from plugins and PluginLoader.dll.

#pragma once

#include <cstdarg>

// Include files from TorqueMath
// It's only necessary to link to the DLL if you actually use functions from the library
#include "../TorqueMath/platform/platform.h"
#include "../TorqueMath/math/mMath.h"

#ifdef _DEBUG
#define DEBUG_PRINTF(fmt, ...) TGE::Con::printf(fmt, __VA_ARGS__)
#define DEBUG_WARNF(fmt, ...)  TGE::Con::warnf(fmt, __VA_ARGS__)
#define DEBUG_ERRORF(fmt, ...) TGE::Con::errorf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINTF(fmt, ...)
#define DEBUG_WARNF(fmt, ...)
#define DEBUG_ERRORF(fmt, ...)
#endif

// Utility macros
#define EXPAND(...) __VA_ARGS__

// Macro for quickly declaring a function pointer that points to a static address
#define FN(rettype, name, args, addr) static const auto name = (rettype (*) args)addr

// Macros for declaring overloaded function pointers
//
// Usage example:
// OVERLOAD_PTR {
//     OVERLOAD_FN(int, (int x, int y), 0x12345);
//     OVERLOAD_FN(int, (int z), 0x23456);
// } foo;
#define OVERLOAD_PTR static struct

// Huge hack to make this work with Intellisense
// If Intellisense is parsing the file, we show it that a () operator exists so that it can help with parameters
// However, when the compiler actually reads it, it'll only see a typecast operator
#ifndef __INTELLISENSE__
// This is what the compiler will see
#define OVERLOAD_FN(rettype, args, addr)                \
	private:                                            \
		typedef rettype (*t_##addr##_) args;            \
	public:                                             \
		inline operator t_##addr##_ () const            \
		{                                               \
			return reinterpret_cast<t_##addr##_>(addr); \
		}
#else
// This is what intellisense will see
#define OVERLOAD_FN(rettype, args, addr)                \
	private:                                            \
		typedef rettype (*t_##addr##_) args;            \
	public:                                             \
		inline rettype operator () args const           \
		{                                               \
		}                                               \
		inline operator t_##addr##_() const             \
		{                                               \
			return reinterpret_cast<t_##addr##_>(addr); \
		}
#endif

// Defines an unnamed pure virtual function
#define UNKVIRT(x) virtual void z_unk##x##_() = 0

// Defines an undefined pure virtual function
#define UNDEFVIRT(name) virtual void z_undef_##name##_() = 0

// Defines a "raw" member function pointer
// Note: IntelliSense will complain if args is empty, but the actual compiler doesn't care
#define RAWMEMBERFN(rettype, name, args, addr) static const auto name = (rettype (__thiscall *) (EXPAND(void *thisobj, EXPAND args)))addr

// Defines a wrapper which calls a member function on a class
// Yes, argument names need to be repeated, but there's not much I can do about that AFAIK
// Note: IntelliSense will complain if args is empty, but the actual compiler doesn't care
#define MEMBERFN(rettype, name, args, argnames, addr)                                                       \
	rettype name args                                                                                       \
	{                                                                                                       \
		return ((rettype (__thiscall *) (EXPAND(void*, EXPAND args)))addr) (EXPAND(this, EXPAND argnames)); \
	}

// Defines a global variable
#define GLOBALVAR(type, name, addr) static type &name = *(type*)addr

// Defines a function which uses the thiscall convention
#define THISFN(rettype, name, args)                                       \
	rettype __fastcall z_thisfn_impl_##name (EXPAND(void*, EXPAND args)); \
	namespace                                                             \
	{                                                                     \
		__declspec(naked) void z_thisfn_jmp_##name args                   \
		{                                                                 \
			__asm mov edx, ecx                                            \
			__asm xor ecx, ecx                                            \
			__asm jmp z_thisfn_impl_##name                                \
		}                                                                 \
	}                                                                     \
	const auto name = (rettype (__thiscall *) args) z_thisfn_jmp_##name;  \
	rettype __fastcall z_thisfn_impl_##name (EXPAND(void*, EXPAND args))

// Class prototypes
class SimObject;
class BaseMatInstance;

// TGE callback types
typedef const char* (*StringCallback)(SimObject *obj, S32 argc, const char *argv[]);
typedef S32         (*IntCallback)   (SimObject *obj, S32 argc, const char *argv[]);
typedef F32         (*FloatCallback) (SimObject *obj, S32 argc, const char *argv[]);
typedef void        (*VoidCallback)  (SimObject *obj, S32 argc, const char *argv[]);
typedef bool        (*BoolCallback)  (SimObject *obj, S32 argc, const char *argv[]);

namespace TGE
{
	class ConsoleObject
	{
	public:
		UNDEFVIRT(getClassRep);
		UNKVIRT(1); // virtual destructor
		// NOTE: On the Mac version, there's another virtual function right here - seems to be another (?) virtual destructor
	};

	class SimObject: public ConsoleObject
	{
	public:
		UNDEFVIRT(processArguments);
		UNDEFVIRT(onAdd);
		UNDEFVIRT(onRemove);
		UNDEFVIRT(onGroupAdd);
		UNDEFVIRT(onGroupRemove);
		UNDEFVIRT(onNameChange);
		UNDEFVIRT(onStaticModified);
		UNDEFVIRT(inspectPreApply);
		UNDEFVIRT(inspectPostApply);
		UNDEFVIRT(onVideoKill);
		UNDEFVIRT(onVideoResurrect);
		UNDEFVIRT(onDeleteNotify);
		UNDEFVIRT(onEditorEnable);
		UNDEFVIRT(onEditorDisable);
		UNDEFVIRT(getEditorClassName);
		UNDEFVIRT(findObject);
		UNDEFVIRT(write);
		UNDEFVIRT(registerLights);
	};

	class NetObject: public SimObject
	{
	public:
		UNDEFVIRT(getUpdatePriority);
		UNDEFVIRT(packUpdate);
		UNDEFVIRT(unpackUpdate);
		UNDEFVIRT(onCameraScopeQuery);
	};

	class SceneObject: public NetObject
	{
	public:
		UNDEFVIRT(disableCollision);
		UNDEFVIRT(enableCollision);
		UNDEFVIRT(isDisplacable);
		UNDEFVIRT(getMomentum);
		UNDEFVIRT(setMomentum);
		UNDEFVIRT(getMass);
		UNDEFVIRT(displaceObject);
		UNDEFVIRT(setTransform);
		UNDEFVIRT(setScale);
		UNDEFVIRT(setRenderTransform);
		UNDEFVIRT(buildConvex);
		UNDEFVIRT(buildPolyList);
		UNDEFVIRT(buildCollisionBSP);
		UNDEFVIRT(castRay);
		UNDEFVIRT(collideBox);
		UNDEFVIRT(getOverlappingZones);
		UNDEFVIRT(getPointZone);
		UNDEFVIRT(renderShadowVolumes);
		UNDEFVIRT(renderObject);
		UNDEFVIRT(prepRenderImage);
		UNDEFVIRT(scopeObject);
		UNDEFVIRT(getMaterialProperty);
		UNDEFVIRT(onSceneAdd);
		UNDEFVIRT(onSceneRemove);
		UNDEFVIRT(transformModelview);
		UNDEFVIRT(transformPosition);
		UNDEFVIRT(computeNewFrustum);
		UNDEFVIRT(openPortal);
		UNDEFVIRT(closePortal);
		UNDEFVIRT(getWSPortalPlane);
		UNDEFVIRT(installLights);
		UNDEFVIRT(uninstallLights);
		UNDEFVIRT(getLightingAmbientColor);
	};

	class GameBase : public SceneObject
	{
	public:
		UNDEFVIRT(onNewDataBlock);
		UNDEFVIRT(processTick);
		UNDEFVIRT(interpolateTick);
		UNDEFVIRT(advanceTime);
		UNDEFVIRT(advancePhysics);
		UNDEFVIRT(getVelocity);
		UNDEFVIRT(getForce);
		UNDEFVIRT(writePacketData);
		UNDEFVIRT(readPacketData);
		UNDEFVIRT(getPacketDataChecksum);
	};

	class ShapeBase : public GameBase
	{
	public:
		MEMBERFN(void, setHidden, (bool hidden), (hidden), 0x4032F1);

		UNDEFVIRT(setImage);
		UNDEFVIRT(onImageRecoil);
		UNDEFVIRT(ejectShellCasing);
		UNDEFVIRT(updateDamageLevel);
		UNDEFVIRT(updateDamageState);
		UNDEFVIRT(blowUp);
		UNDEFVIRT(onMount);
		UNDEFVIRT(onUnmount);
		UNDEFVIRT(onImpact_SceneObject_Point3F);
		UNDEFVIRT(onImpact_Point3F);
		UNDEFVIRT(controlPrePacketSend);
		UNDEFVIRT(setEnergyLevel);
		UNDEFVIRT(mountObject);
		UNDEFVIRT(mountImage);
		UNDEFVIRT(unmountImage);
		UNDEFVIRT(getMuzzleVector);
		UNDEFVIRT(getCameraParameters);
		virtual void getCameraTransform(F32 *pos, MatrixF *mat) = 0;
		UNDEFVIRT(getEyeTransform);
		UNDEFVIRT(getRetractionTransform);
		UNDEFVIRT(getMountTransform);
		UNDEFVIRT(getMuzzleTransform);
		UNDEFVIRT(getImageTransform_uint_MatrixF);
		UNDEFVIRT(getImageTransform_uint_int_MatrixF);
		UNDEFVIRT(getImageTransform_uint_constchar_MatrixF);
		UNDEFVIRT(getRenderRetractionTransform);
		UNDEFVIRT(getRenderMountTransform);
		UNDEFVIRT(getRenderMuzzleTransform);
		UNDEFVIRT(getRenderImageTransform_uint_MatrixF);
		UNDEFVIRT(getRenderImageTransform_uint_int_MatrixF);
		UNDEFVIRT(getRenderImageTransform_uint_constchar_MatrixF);
		UNDEFVIRT(getRenderMuzzleVector);
		UNDEFVIRT(getRenderMuzzlePoint);
		UNDEFVIRT(getRenderEyeTransform);
		UNDEFVIRT(getDamageFlash);
		UNDEFVIRT(setDamageFlash);
		UNDEFVIRT(getWhiteOut);
		UNDEFVIRT(setWhiteOut);
		UNDEFVIRT(getInvincibleEffect);
		UNDEFVIRT(setupInvincibleEffect);
		UNDEFVIRT(updateInvincibleEffect);
		UNDEFVIRT(setVelocity);
		UNDEFVIRT(applyImpulse);
		UNDEFVIRT(setControllingClient);
		UNDEFVIRT(setControllingObject);
		UNDEFVIRT(getControlObject);
		UNDEFVIRT(setControlObject);
		UNDEFVIRT(getCameraFov);
		UNDEFVIRT(getDefaultCameraFov);
		UNDEFVIRT(setCameraFov);
		UNDEFVIRT(isValidCameraFov);
		UNDEFVIRT(renderMountedImage);
		UNDEFVIRT(renderImage);
		UNDEFVIRT(calcClassRenderData);
		UNDEFVIRT(onCollision);
		UNDEFVIRT(getSurfaceFriction);
		UNDEFVIRT(getBounceFriction);
		UNDEFVIRT(setHidden);
	};

	class DisplayDevice
	{
	public:
		virtual void initDevice() = 0;
		virtual bool activate(U32 width, U32 height, U32 bpp, bool fullScreen) = 0;
		virtual void shutdown() = 0;
		virtual bool setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true) = 0;
		virtual bool setResolution(U32 width, U32 height, U32 bpp) = 0;
		virtual bool toggleFullScreen() = 0;
		virtual void swapBuffers() = 0;
		virtual const char *getDriverInfo() = 0;
		virtual bool getGammaCorrection(F32 &g) = 0;
		virtual bool setGammaCorrection(F32 g) = 0;
		virtual bool setVerticalSync(bool on) = 0;
	};

	class OpenGLDevice : public DisplayDevice
	{
	};

	class Marble: public ShapeBase
	{
	public:
		// No virtual functions
	};

	struct Collision
	{
		SceneObject* object;
		Point3F point;
		VectorF normal;
		BaseMatInstance* material;

		// Face and Face dot are currently only set by the extrudedPolyList
		// clipper.  Values are otherwise undefined.
		U32 face;                  // Which face was hit
		F32 faceDot;               // -Dot of face with poly normal
		F32 distance;

		Collision() :
			object(NULL),
			material(NULL)
		{
		}
	};

	/// Extension of the collision structure to allow use with raycasting.
	/// @see Collision
	struct RayInfo : public Collision
	{
		// The collision struct has object, point, normal & material.

		/// Distance along ray to contact point.
		F32 t;

		/// Set the point of intersection according to t and the given ray.
		///
		/// Several pieces of code will not use ray information but rather rely
		/// on contact points directly, so it is a good thing to always set
		/// this in castRay functions.
		void setContactPoint(const Point3F& start, const Point3F& end)
		{
			Point3F startToEnd = end - start;
			startToEnd *= t;
			point = startToEnd + start;
		}
	};

	class Container
	{
	public:
		MEMBERFN(bool, castRay, (const Point3F &start, const Point3F &end, U32 mask, RayInfo *info), (start, end, mask, info), 0x403652);
	};

	class Stream
	{
	public:
		virtual ~Stream() = 0;
		virtual bool _read(U32 size, void *buf) = 0;
		virtual bool _write(U32 size, const void *buf) = 0;
		virtual bool hasCapability(int capability) = 0; // TODO: Define Stream::Capability enum
		virtual U32 getPosition() = 0;
		virtual bool setPosition(U32 pos);
		virtual U32 getStreamSize() = 0;
		virtual void readString(char *str) = 0;
		virtual void writeString(const char *str, S32 maxLength = 255) = 0;
	};

	class FileStream : Stream
	{
	public:
		MEMBERFN(bool, open, (const char *path, int accessMode), (path, accessMode), 0x405F10);
	};

	// Console enums
	namespace ConsoleLogEntry
	{
		enum Level
		{
			Normal = 0,
			Warning,
			Error,
			NUM_CLASS
		};

		enum Type
		{
			General = 0,
			Assert,
			Script,
			GUI,
			Network,
			NUM_TYPE
		};
	}

	namespace Con
	{
		// Initialization
		FN(void, init, (), 0x406645);
		
		// Logging
		FN(void, printf, (const char *fmt, ...), 0x405984);
		FN(void, _printf, (ConsoleLogEntry::Level level, ConsoleLogEntry::Type type, const char *fmt, va_list argptr), 0x42D1F0);
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *fmt, ...),                             0x404B5B);
			OVERLOAD_FN(void, (ConsoleLogEntry::Type type, const char *fmt, ...), 0x408503);
		} warnf;
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *fmt, ...),                             0x405763);
			OVERLOAD_FN(void, (ConsoleLogEntry::Type type, const char *fmt, ...), 0x401023);
		} errorf;

		// addCommand()
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                     0x404F2A);
			OVERLOAD_FN(void, (const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                       0x407B8A);
			OVERLOAD_FN(void, (const char *name, IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                        0x4044AD);
			OVERLOAD_FN(void, (const char *name, FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                      0x408972);
			OVERLOAD_FN(void, (const char *name, BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                       0x407527);
			OVERLOAD_FN(void, (const char *nsName, const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs), 0x40243C);
			OVERLOAD_FN(void, (const char *nsName, const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs),   0x4080D0);
			OVERLOAD_FN(void, (const char *nsName, const char *name, IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs),    0x4084C7);
			OVERLOAD_FN(void, (const char *nsName, const char *name, FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs),  0x404ADE);
			OVERLOAD_FN(void, (const char *nsName, const char *name, BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs),   0x403698);
		} addCommand;

		// Variables
		FN(void, setVariable,      (const char *name, const char *value), 0x408684);
		FN(void, setLocalVariable, (const char *name, const char *value), 0x402AC2);
		FN(void, setBoolVariable,  (const char *name, bool value),        0x405A38);
		FN(void, setIntVariable,   (const char *name, S32 value),         0x4064AB);
		FN(void, setFloatVariable, (const char *name, F32 value),         0x4049E9);

		// Misc
		FN(const char*, execute,              (S32 argc, const char *argv[]),                         0x403E13);
		FN(const char*, evaluate,             (const char *string, bool echo, const char *fileName),  0x401E56);
		FN(const char*, evaluatef,            (const char* string, ...),                              0x40713F);
		FN(char*,       getReturnBuffer,      (U32 bufferSize),                                       0x407211);
		FN(bool,        expandScriptFilename, (char *filename, U32 size, const char *src),            0x402B35);
	}

	namespace ParticleEngine
	{
		// Initialization
		FN(void, init, (), 0x4093A4);
	}

	/*namespace ShapeBase
	{
		// Shadows (thiscall - do not call directly)
		THISFN(void, renderShadow, (float, float), 0x406FF0);
	}

	namespace TSStatic
	{
		// Shadows (thiscall - do not call directly)
		THISFN(void, renderShadow, (float, float), 0x40524F);
	}*/

	namespace Sim
	{
		FN(SimObject*, findObject, (const char *name), 0x405308);
	}

	namespace Net
	{
		FN(bool, init, (), 0x404C32);
	}

	namespace Members
	{
		namespace OpenGLDevice
		{
			RAWMEMBERFN(void, initDevice, (), 0x4033BE);
		}

		namespace Marble
		{
			RAWMEMBERFN(void, doPowerUp, (int id), 0x405F51);
		}

		namespace FileStream
		{
			RAWMEMBERFN(bool, open, (const char *path, int accessMode), 0x405F10);
		}
	}

	FN(void, clientProcess, (U32 timeDelta), 0x403B2A);

	// Global variables
	GLOBALVAR(Container, gClientContainer, 0x6E1838);
	GLOBALVAR(Container, gServerContainer, 0x6E1760);
}

// ConsoleFunction() can't be used from inside PluginLoader.dll without crashes
#ifndef IN_PLUGIN_LOADER

namespace TGE
{
	// Psuedo class used to implement the ConsoleFunction macro.
	class _ConsoleConstructor
	{
	public:
		template<typename... Args>
		_ConsoleConstructor(Args... args) { Con::addCommand(args...); }
	};
}

// Defines a console function.
#define ConsoleFunction(name, returnType, minArgs, maxArgs, usage)                         \
	static returnType c##name(SimObject *, S32, const char **argv);                        \
	static TGE::_ConsoleConstructor g##name##obj(#name, c##name, usage, minArgs, maxArgs); \
	static returnType c##name(SimObject *, S32 argc, const char **argv)

#endif