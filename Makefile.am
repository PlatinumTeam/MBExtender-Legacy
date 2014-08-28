## It's much easier (AFAIK) to just recurse into the include folder than to
## define dir variables for every single include subdirectory
SUBDIRS = include

AM_LDFLAGS = -module -avoid-version
AM_CXXFLAGS = -Iinclude -fvisibility=hidden

helperdir = $(bindir)
plugindir = $(bindir)/plugins

bin_PROGRAMS = MBExtender

if LINUX
MBExtender_SOURCES = \
	src/MBExtender/linux/main-linux.cpp
endif
if WINDOWS
MBExtender_SOURCES = \
	src/MBExtender/win32/main-win32.cpp \
	src/MBExtender/win32/DLLInjection.cpp \
	src/MBExtender/win32/DLLInjection.h
endif

helper_LTLIBRARIES = \
	PluginLoader.la \
	TorqueLib.la
	
plugin_LTLIBRARIES = \
	TestPlugin.la \
	RandomPowerupsDemo.la \
	ExternalConsole.la \
	MovingPlatformsFix.la \
	MultiplayerFix.la

PluginLoader_la_SOURCES = \
	src/PluginLoader/CodeAllocator.cpp \
	src/PluginLoader/CodeAllocator.h \
	src/PluginLoader/CodeInjectionStream.cpp \
	src/PluginLoader/CodeInjectionStream.h \
	src/PluginLoader/FuncInterceptor.cpp \
	src/PluginLoader/FuncInterceptor.h \
	src/PluginLoader/PluginLoader.cpp \
	src/PluginLoader/TrampolineGenerator.cpp \
	src/PluginLoader/TrampolineGenerator.h \
	src/PluginLoader/BasicPluginInterface.cpp \
	src/PluginLoader/BasicPluginInterface.h \
	src/PluginLoader/SharedObject.h \
	src/PluginLoader/Memory.h \
	src/PluginLoader/LDE64.h
	
if WINDOWS
PluginLoader_la_SOURCES += \
	src/PluginLoader/win32/LDE64-masm.asm \
	src/PluginLoader/win32/Memory-win32.cpp \
	src/PluginLoader/win32/SharedObject-win32.cpp \
	src/PluginLoader/win32/main-win32.cpp
else
PluginLoader_la_SOURCES += \
	src/PluginLoader/unix/LDE64-as.s \
	src/PluginLoader/unix/Memory-unix.cpp \
	src/PluginLoader/unix/SharedObject-unix.cpp \
	src/PluginLoader/unix/main-unix.cpp
endif
	
PluginLoader_la_CXXFLAGS = -DIN_PLUGIN_LOADER $(AM_CXXFLAGS)

TorqueLib_la_SOURCES = \
	src/TorqueLib/TorqueLib.cpp \
	src/TorqueLib/math/mAngAxis.cpp \
	src/TorqueLib/math/mathUtils.cpp \
	src/TorqueLib/math/mBox.cpp \
	src/TorqueLib/math/mEase.cpp \
	src/TorqueLib/math/mMathAltivec.cpp \
	src/TorqueLib/math/mMathAMD.cpp \
	src/TorqueLib/math/mMath_C.cpp \
	src/TorqueLib/math/mMathSSE.cpp \
	src/TorqueLib/math/mMatrix.cpp \
	src/TorqueLib/math/mOrientedBox.cpp \
	src/TorqueLib/math/mPlane.cpp \
	src/TorqueLib/math/mPlaneTransformer.cpp \
	src/TorqueLib/math/mPoint.cpp \
	src/TorqueLib/math/mQuat.cpp \
	src/TorqueLib/math/mRandom.cpp \
	src/TorqueLib/math/mRect.cpp \
	src/TorqueLib/math/mSolver.cpp \
	src/TorqueLib/math/mSphere.cpp \
	src/TorqueLib/math/util/quadTransforms.cpp
	
TorqueLib_la_CXXFLAGS = -Iinclude/TorqueLib/math -Iinclude/TorqueLib/math/util $(AM_CXXFLAGS)

TestPlugin_la_SOURCES = \
	plugins/TestPlugin/TestPlugin.cpp

RandomPowerupsDemo_la_SOURCES = \
	plugins/RandomPowerupsDemo/RandomPowerupsDemo.cpp
	
ExternalConsole_la_SOURCES = \
	plugins/ExternalConsole/ExternalConsole.cpp
	
MovingPlatformsFix_la_SOURCES = \
	plugins/MovingPlatformsFix/MovingPlatformsFix.cpp
	
MultiplayerFix_la_SOURCES = \
	plugins/MultiplayerFix/MultiplayerFix.cpp
	
## Only compile FakeIgnition on Linux
if LINUX
plugin_LTLIBRARIES += FakeIgnition.la
FakeIgnition_la_SOURCES = \
	plugins/FakeIgnition/FakeIgnition.cpp
endif

## Hook to copy TorqueLib to libdir and delete .la files (they aren't necessary)
install-data-hook:
	mkdir -p $(libdir) && cp $(helperdir)/TorqueLib.* $(libdir)
	cd $(helperdir) && rm -f $(helper_LTLIBRARIES)
	cd $(plugindir) && rm -f $(plugin_LTLIBRARIES)
