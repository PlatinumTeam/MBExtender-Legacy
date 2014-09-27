// TGE.h: Interface to core TGE functions.
// Usable both from plugins and PluginLoader.dll.

#ifndef TORQUELIB_TGE_H
#define TORQUELIB_TGE_H

#include <cstdarg>

#include "platform/platform.h"
#include "math/mMath.h"
#include "util/tVector.h"

#ifdef _DEBUG
#define DEBUG_PRINTF(fmt, ...) TGE::Con::printf(fmt, __VA_ARGS__)
#define DEBUG_WARNF(fmt, ...)  TGE::Con::warnf(fmt, __VA_ARGS__)
#define DEBUG_ERRORF(fmt, ...) TGE::Con::errorf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINTF(fmt, ...)
#define DEBUG_WARNF(fmt, ...)
#define DEBUG_ERRORF(fmt, ...)
#endif

// Determine macros and addresses to use based on host OS
#ifdef _WIN32
#include "win32/InterfaceMacros-win32.h"
#include "win32/Addresses-win32.h"
#endif
#ifdef __linux
#include "linux/InterfaceMacros-linux.h"
#include "linux/Addresses-linux.h"
#endif

namespace TGE
{
	// Class prototypes
	class SimObject;
	class BaseMatInstance;
	class GameConnection;
	class Camera;
	class ResourceObject;
	struct Move;
}

// TGE callback types
typedef const char* (*StringCallback)(TGE::SimObject *obj, S32 argc, const char *argv[]);
typedef S32         (*IntCallback)   (TGE::SimObject *obj, S32 argc, const char *argv[]);
typedef F32         (*FloatCallback) (TGE::SimObject *obj, S32 argc, const char *argv[]);
typedef void        (*VoidCallback)  (TGE::SimObject *obj, S32 argc, const char *argv[]);
typedef bool        (*BoolCallback)  (TGE::SimObject *obj, S32 argc, const char *argv[]);

typedef U32 SimObjectId;

namespace TGE
{
	class ConsoleObject
	{
	public:
		VTABLE(TGEOFF_CONSOLEOBJECT_VTABLE);
		
		UNDEFVIRT(getClassRep);
		VIRTDTOR(~ConsoleObject, 1);
		// NOTE: On the Mac version, there's another virtual function right here - seems to be another (?) virtual destructor
	};

	class SimObject: public ConsoleObject
	{
	public:
		GETTERFN(SimObjectId, getId, TGEOFF_SIMOBJECT_ID);
		MEMBERFNSIMP(const char*, getIdString, TGEADDR_SIMOBJECT_GETIDSTRING);
		MEMBERFN(void, setHidden, (bool hidden), (hidden), TGEADDR_SIMOBJECT_SETHIDDEN);

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
		VIRTFNSIMP(void, disableCollision, TGEVIRT_SCENEOBJECT_DISABLECOLLISION);
		VIRTFNSIMP(void, enableCollision, TGEVIRT_SCENEOBJECT_ENABLECOLLISION);
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
		GETTERFN(GameConnection*, getControllingClient, TGEOFF_GAMEBASE_CONTROLLINGCLIENT);

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
		VIRTFN(void, getCameraTransform, (F32 *pos, MatrixF *mat), (pos, mat), TGEVIRT_SHAPEBASE_GETCAMERATRANSFORM);
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

	// TODO: Convert this to use VIRTFN
	/*class DisplayDevice
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
	};*/

	class Marble: public ShapeBase
	{
	};
	
	class Camera: public ShapeBase
	{
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
		MEMBERFN(bool, castRay, (const Point3F &start, const Point3F &end, U32 mask, RayInfo *info), (start, end, mask, info), TGEADDR_CONTAINER_CASTRAY);
	};

	class Stream
	{
	public:
		VTABLE(TGEOFF_STREAM_VTABLE);
		
		/// Status constants for the stream
		enum StreamStatus
		{
			Ok = 0,      ///< Ok!
			IOError,     ///< Read or Write error
			EOS,         ///< End of Stream reached (mostly for reads)
			IllegalCall, ///< An unsupported operation used. Always w/ accompanied by AssertWarn
			Closed,      ///< Tried to operate on a closed stream (or detached filter)
			UnknownError ///< Catchall
		};
		
		GETTERFN(StreamStatus, getStatus, TGEOFF_STREAM_STATUS);

		VIRTDTOR(~Stream, TGEVIRT_STREAM_DTOR);
		VIRTFN(bool, _read, (U32 size, void *buf), (size, buf), TGEVIRT_STREAM__READ);
		VIRTFN(bool, _write, (U32 size, const void *buf), (size, buf), TGEVIRT_STREAM__WRITE);
		VIRTFN(bool, hasCapability, (int capability), (capability), TGEVIRT_STREAM_HASCAPABILITY);
		VIRTFNSIMP(U32, getPosition, TGEVIRT_STREAM_GETPOSITION);
		VIRTFN(bool, setPosition, (U32 pos), (pos), TGEVIRT_STREAM_SETPOSITION);
		VIRTFNSIMP(U32, getStreamSize, TGEVIRT_STREAM_GETSTREAMSIZE);
		VIRTFN(void, readString, (char *str), (str), TGEVIRT_STREAM_READSTRING);
		VIRTFN(void, writeString, (const char *str, S32 maxLength), (str, maxLength), TGEVIRT_STREAM_WRITESTRING);
	};

	class FileStream : public Stream
	{
	public:
		MEMBERFN(bool, open, (const char *path, int accessMode), (path, accessMode), TGEADDR_FILESTREAM_OPEN);
	};

	class File
	{
	public:
		/// What is the status of our file handle?
		enum FileStatus
		{
			Ok = 0,      ///< Ok!
			IOError,     ///< Read or Write error
			EOS,         ///< End of Stream reached (mostly for reads)
			IllegalCall, ///< An unsupported operation used. Always accompanied by AssertWarn
			Closed,      ///< Tried to operate on a closed stream (or detached filter)
			UnknownError ///< Catchall
		};

		/// How are we accessing the file?
		enum AccessMode
		{
			Read = 0,       ///< Open for read only, starting at beginning of file.
			Write = 1,      ///< Open for write only, starting at beginning of file; will blast old contents of file.
			ReadWrite = 2,  ///< Open for read-write.
			WriteAppend = 3 ///< Write-only, starting at end of file.
		};

		/// Flags used to indicate what we can do to the file.
		enum Capability
		{
			FileRead = 1 << 0,
			FileWrite = 1 << 1
		};
		
		MEMBERFN(void, setStatus, (FileStatus status), (status), TGEADDR_FILE_SETSTATUS_1); // Technically supposed to be protected
		GETTERFN(void*, getHandle, TGEOFF_FILE_HANDLE);
		SETTERFN(void*, setHandle, TGEOFF_FILE_HANDLE);
		GETTERFN(Capability, getCapabilities, TGEOFF_FILE_CAPABILITIES);
		SETTERFN(Capability, setCapabilities, TGEOFF_FILE_CAPABILITIES);

		MEMBERFN(FileStatus, open, (const char *filename, const AccessMode openMode), (filename, openMode), TGEADDR_FILE_OPEN);
		MEMBERFNSIMP(U32, getPosition, TGEADDR_FILE_GETPOSITION);
		MEMBERFN(FileStatus, setPosition, (S32 position, bool absolutePos), (position, absolutePos), TGEADDR_FILE_SETPOSITION);
		MEMBERFNSIMP(U32, getSize, TGEADDR_FILE_GETSIZE);
		MEMBERFNSIMP(FileStatus, flush, TGEADDR_FILE_FLUSH);
		MEMBERFNSIMP(FileStatus, close, TGEADDR_FILE_CLOSE);
		MEMBERFNSIMP(FileStatus, getStatus, TGEADDR_FILE_GETSTATUS);
		MEMBERFN(FileStatus, read, (U32 size, char *dst, U32 *bytesRead), (size, dst, bytesRead), TGEADDR_FILE_READ);
		MEMBERFN(FileStatus, write, (U32 size, const char *src, U32 *bytesWritten), (size, src, bytesWritten), TGEADDR_FILE_WRITE);
	};

	class ResManager
	{
	public:
		MEMBERFN(Stream*, openStream, (const char *path), (path), TGEADDR_RESMANAGER_OPENSTREAM_STR);
		MEMBERFN(Stream*, openStream, (ResourceObject *obj), (obj), TGEADDR_RESMANAGER_OPENSTREAM_RESOURCEOBJECT);
		MEMBERFN(void, closeStream, (Stream *stream), (stream), TGEADDR_RESMANAGER_CLOSESTREAM);
		MEMBERFN(ResourceObject*, find, (const char *path), (path), TGEADDR_RESMANAGER_FIND);
		MEMBERFN(U32, getSize, (const char *path), (path), TGEADDR_RESMANAGER_GETSIZE);
		MEMBERFN(bool, getCrc, (const char *path, U32 &crc, U32 initialValue), (path, crc, initialValue), TGEADDR_RESMANAGER_GETCRC);
		MEMBERFN(void, searchPath, (const char *path), (path), TGEADDR_RESMANAGER_SEARCHPATH);
		MEMBERFN(bool, setModZip, (const char *path), (path), TGEADDR_RESMANAGER_SETMODZIP);
	};

	class ConnectionProtocol
	{
	private:
		// TODO: Clean this up, this is just so casting SimObject* to NetConnection* works properly
		unsigned char fields[0x9C];
		virtual void placeholder() = 0; // Force virtual inheritance
	};
	
	GLOBALVAR(TGE::SimObject*, mServerConnection, TGEADDR_MSERVERCONNECTION);

	class NetConnection : public ConnectionProtocol, public SimObject
	{
	public:
		static NetConnection *getConnectionToServer()
		{
			return static_cast<TGE::NetConnection*>(mServerConnection);
		}
	};

	class GameConnection : public NetConnection
	{
	};

	class PathedInterior : public GameBase
	{
	public:
		MEMBERFN(void, advance, (double delta), (delta), TGEADDR_PATHEDINTERIOR_ADVANCE);
		MEMBERFN(void, computeNextPathStep, (U32 delta), (delta), TGEADDR_PATHEDINTERIOR_COMPUTENEXTPATHSTEP);
	};

	class AbstractClassRep
	{
	public:
		GETTERFN(const char*, getClassName, TGEOFF_ABSTRACTCLASSREP_CLASSNAME);
	};

	class _StringTable
	{
	public:
		MEMBERFN(const char*, insert, (const char *string, bool caseSens), (string, caseSens), TGEADDR__STRINGTABLE_INSERT);
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
		FN(void, init, (), TGEADDR_CON_INIT);
		
		// Logging
		FN(void, printf, (const char *fmt, ...), TGEADDR_CON_PRINTF);
		FN(void, _printf, (ConsoleLogEntry::Level level, ConsoleLogEntry::Type type, const char *fmt, va_list argptr), TGEADDR_CON__PRINTF);
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *fmt, ...),                             TGEADDR_CON_WARNF_1V);
			OVERLOAD_FN(void, (ConsoleLogEntry::Type type, const char *fmt, ...), TGEADDR_CON_WARNF_2V);
		} warnf;
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *fmt, ...),                             TGEADDR_CON_ERRORF_1V);
			OVERLOAD_FN(void, (ConsoleLogEntry::Type type, const char *fmt, ...), TGEADDR_CON_ERRORF_2V);
		} errorf;

		// addCommand()
		OVERLOAD_PTR {
			OVERLOAD_FN(void, (const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                     TGEADDR_CON_ADDCOMMAND_5_STRING);
			OVERLOAD_FN(void, (const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                       TGEADDR_CON_ADDCOMMAND_5_VOID);
			OVERLOAD_FN(void, (const char *name, IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                        TGEADDR_CON_ADDCOMMAND_5_INT);
			OVERLOAD_FN(void, (const char *name, FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                      TGEADDR_CON_ADDCOMMAND_5_FLOAT);
			OVERLOAD_FN(void, (const char *name, BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs),                       TGEADDR_CON_ADDCOMMAND_5_BOOL);
			OVERLOAD_FN(void, (const char *nsName, const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs), TGEADDR_CON_ADDCOMMAND_6_STRING);
			OVERLOAD_FN(void, (const char *nsName, const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs),   TGEADDR_CON_ADDCOMMAND_6_VOID);
			OVERLOAD_FN(void, (const char *nsName, const char *name, IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs),    TGEADDR_CON_ADDCOMMAND_6_INT);
			OVERLOAD_FN(void, (const char *nsName, const char *name, FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs),  TGEADDR_CON_ADDCOMMAND_6_FLOAT);
			OVERLOAD_FN(void, (const char *nsName, const char *name, BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs),   TGEADDR_CON_ADDCOMMAND_6_BOOL);
		} addCommand;

		// Variables
		FN(void, setVariable,      (const char *name, const char *value), TGEADDR_CON_SETVARIABLE);
		FN(void, setLocalVariable, (const char *name, const char *value), TGEADDR_CON_SETLOCALVARIABLE);
		FN(void, setBoolVariable,  (const char *name, bool value),        TGEADDR_CON_SETBOOLVARIABLE);
		FN(void, setIntVariable,   (const char *name, S32 value),         TGEADDR_CON_SETINTVARIABLE);
		FN(void, setFloatVariable, (const char *name, F32 value),         TGEADDR_CON_SETFLOATVARIABLE);

		// Misc
		FN(const char*, execute,              (S32 argc, const char *argv[]),                        TGEADDR_CON_EXECUTE);
		FN(const char*, evaluate,             (const char *string, bool echo, const char *fileName), TGEADDR_CON_EVALUATE);
		FN(const char*, evaluatef,            (const char* string, ...),                             TGEADDR_CON_EVALUATEF);
		FN(char*,       getReturnBuffer,      (U32 bufferSize),                                      TGEADDR_CON_GETRETURNBUFFER);
		FN(bool,        expandScriptFilename, (char *filename, U32 size, const char *src),           TGEADDR_CON_EXPANDSCRIPTFILENAME);
	}

	namespace Platform
	{
		FN(bool, dumpPath, (const char *path, Vector<FileInfo>& fileVector), TGEADDR_PLATFORM_DUMPPATH);
		FN(const char*, getWorkingDirectory, (), TGEADDR_PLATFORM_GETWORKINGDIRECTORY);
		FN(bool, isSubDirectory, (const char *parent, const char *child), TGEADDR_PLATFORM_ISSUBDIRECTORY);
		FN(bool, getFileTimes, (const char *path, FileTime *createTime, FileTime *modifyTime), TGEADDR_PLATFORM_GETFILETIMES);
	}

	namespace Namespace
	{
		FN(void, init, (), TGEADDR_NAMESPACE_INIT);
	}

	namespace ParticleEngine
	{
		// Initialization
		FN(void, init, (), TGEADDR_PARTICLEENGINE_INIT);
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
		FN(SimObject*, findObject, (const char *name), TGEADDR_SIM_FINDOBJECT);
	}

	namespace Net
	{
		FN(bool, init, (), TGEADDR_NET_INIT);
	}

	namespace Members
	{
		/*namespace OpenGLDevice
		{
			RAWMEMBERFNSIMP(TGE::OpenGLDevice, void, initDevice, 0x4033BE);
		}*/

		namespace Marble
		{
			RAWMEMBERFN(TGE::Marble, void, doPowerUp, (int id), TGEADDR_MARBLE_DOPOWERUP);
			RAWMEMBERFN(TGE::Marble, void, advancePhysics, (const Move *move, U32 delta), TGEADDR_MARBLE_ADVANCEPHYSICS);
		}

		namespace FileStream
		{
			RAWMEMBERFN(TGE::FileStream, bool, open, (const char *path, int accessMode), TGEADDR_FILESTREAM_OPEN);
		}

		namespace PathedInterior
		{
			RAWMEMBERFN(TGE::PathedInterior, void, advance, (double delta), TGEADDR_PATHEDINTERIOR_ADVANCE);
			RAWMEMBERFN(TGE::PathedInterior, void, computeNextPathStep, (U32 delta), TGEADDR_PATHEDINTERIOR_COMPUTENEXTPATHSTEP);
		}

		namespace AbstractClassRep
		{
			FN(void, initialize, (), TGEADDR_ABSTRACTCLASSREP_INITIALIZE);
		}

		namespace File
		{
			RAWMEMBERFN(TGE::File, TGE::File::FileStatus, open, (const char *filename, const TGE::File::AccessMode openMode), TGEADDR_FILE_OPEN);
			RAWMEMBERFNSIMP(TGE::File, U32, getPosition, TGEADDR_FILE_GETPOSITION);
			RAWMEMBERFN(TGE::File, TGE::File::FileStatus, setPosition, (S32 position, bool absolutePos), TGEADDR_FILE_SETPOSITION);
			RAWMEMBERFNSIMP(TGE::File, U32, getSize, TGEADDR_FILE_GETSIZE);
			RAWMEMBERFNSIMP(TGE::File, TGE::File::FileStatus, flush, TGEADDR_FILE_FLUSH);
			RAWMEMBERFNSIMP(TGE::File, TGE::File::FileStatus, close, TGEADDR_FILE_CLOSE);
			RAWMEMBERFNSIMP(TGE::File, TGE::File::FileStatus, getStatus, TGEADDR_FILE_GETSTATUS);
			RAWMEMBERFN(TGE::File, void, setStatus, (TGE::File::FileStatus status), TGEADDR_FILE_SETSTATUS_1); // Technically supposed to be protected
			RAWMEMBERFN(TGE::File, TGE::File::FileStatus, read, (U32 size, char *dst, U32 *bytesRead), TGEADDR_FILE_READ);
			RAWMEMBERFN(TGE::File, TGE::File::FileStatus, write, (U32 size, const char *src, U32 *bytesWritten), TGEADDR_FILE_WRITE);
			RAWMEMBERFNSIMP(TGE::File, void, destructor_, TGEADDR_FILE_DTOR);
		}
		
		namespace Camera
		{
			RAWMEMBERFN(TGE::Camera, void, advancePhysics, (const TGE::Move *move, U32 delta), TGEADDR_CAMERA_ADVANCEPHYSICS);
		}
		
		namespace ResManager
		{
			// TODO: Allow overloading raw member pointers
			/*RAWMEMBERFN(TGE::ResManager, Stream*, openStream, (const char *path), 0x407E37);
			RAWMEMBERFN(TGE::ResManager, Stream*, openStream, (ResourceObject *obj), 0x4079EB);*/
			RAWMEMBERFN(TGE::ResManager, void, closeStream, (Stream *stream), TGEADDR_RESMANAGER_CLOSESTREAM);
			RAWMEMBERFN(TGE::ResManager, ResourceObject*, find, (const char *path), TGEADDR_RESMANAGER_FIND);
			RAWMEMBERFN(TGE::ResManager, U32, getSize, (const char *path), TGEADDR_RESMANAGER_GETSIZE);
			RAWMEMBERFN(TGE::ResManager, bool, getCrc, (const char *path, U32 &crc, U32 initialValue), TGEADDR_RESMANAGER_GETCRC);
			RAWMEMBERFN(TGE::ResManager, void, searchPath, (const char *path), TGEADDR_RESMANAGER_SEARCHPATH);
			RAWMEMBERFN(TGE::ResManager, bool, setModZip, (const char *path), TGEADDR_RESMANAGER_SETMODZIP);
		}
	}

	FN(void, clientProcess, (U32 timeDelta), TGEADDR_CLIENTPROCESS);

	// Platform functions
	FN(void, dSprintf, (char *buffer, size_t bufferSize, const char *format, ...), TGEADDR_DSPRINTF);
	FN(void, dFree, (void *ptr), TGEADDR_DFREE);
	FN(void, dQsort, (void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void*, const void*)), TGEADDR_DQSORT);
	FN(bool, VectorResize, (U32 *aSize, U32 *aCount, void **arrayPtr, U32 newCount, U32 elemSize), TGEADDR_VECTORRESIZE);

	// Global variables
	GLOBALVAR(Container, gClientContainer, TGEADDR_GCLIENTCONTAINER);
	GLOBALVAR(Container, gServerContainer, TGEADDR_GSERVERCONTAINER);
	GLOBALVAR(_StringTable*, StringTable, TGEADDR_STRINGTABLE);
	GLOBALVAR(ResManager*, ResourceManager, TGEADDR_RESOURCEMANAGER);
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
	static returnType c##name(TGE::SimObject *, S32, const char **argv);                   \
	static TGE::_ConsoleConstructor g##name##obj(#name, c##name, usage, minArgs, maxArgs); \
	static returnType c##name(TGE::SimObject *, S32 argc, const char **argv)

#endif // IN_PLUGIN_LOADER

#endif // TORQUELIB_TGE_H
