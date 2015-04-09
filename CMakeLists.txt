cmake_minimum_required (VERSION 2.8.8)
include (CheckIncludeFiles)
project (MBExtender C CXX)

if (UNIX)
	# Set Unix-specific flags
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable -std=c++03 -fvisibility=hidden")
	enable_language (ASM)

	# Force 32 bit
	# NOTE: This causes issues with Xcode. You still have to explicitly set a 32-bit target.
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set (CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -m32")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
	set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32")
elseif (MSVC)
	# Set VC++ settings
	cmake_policy (SET CMP0054 NEW) # CMake complains otherwise when we call enable_language
	enable_language (ASM_MASM)
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO") # SAFESEH needs to be disabled because LDE64 doesn't support it
	set (CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "Toolset to compile with") # Force XP (this is kinda hackish, but the CMake GUI doesn't let you specify a toolset yet)

	# Force use of static runtime library
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif ()

# Set OS X dylibs to be loaded from the executable directory
set (CMAKE_MACOSX_RPATH ON)
set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set (CMAKE_INSTALL_NAME_DIR "@executable_path")

# Check for cpuid.h or intrin.h in order to use cpuid
check_include_files (cpuid.h HAVE_CPUID_H)
check_include_files (intrin.h HAVE_INTRIN_H)
if (NOT HAVE_CPUID_H AND NOT HAVE_INTRIN_H)
	message (FATAL_ERROR "Either cpuid.h or intrin.h is required.")
endif ()

# Generate a config.h file
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
add_definitions(-DHAVE_CONFIG_H)

# Register public include folder and build folder
include_directories (include ${PROJECT_BINARY_DIR})

# Launcher
if (UNIX)
	add_executable (MBExtender src/MBExtender/unix/main-unix.cpp)
elseif (WIN32)
	add_executable (MBExtender WIN32
		src/MBExtender/win32/main-win32.cpp
		src/MBExtender/win32/DLLInjection.cpp
	)
endif ()

# FakeVorbis (Windows only)
if (WIN32)
	add_library (FakeVorbis SHARED src/FakeVorbis/init.cpp src/FakeVorbis/vorbis.cpp)
	if (MSVC)
		set_property (TARGET FakeVorbis APPEND PROPERTY LINK_FLAGS "/DEF:\"${PROJECT_SOURCE_DIR}/src/FakeVorbis/vorbis.def\"")
	endif ()
endif ()

# Plugin loader
set (PLUGINLOADER_SRC
	src/PluginLoader/BasicPluginInterface.cpp
	src/PluginLoader/CodeAllocator.cpp
	src/PluginLoader/CodeInjectionStream.cpp
	src/PluginLoader/FuncInterceptor.cpp
	src/PluginLoader/PluginLoader.cpp
	src/PluginLoader/TrampolineGenerator.cpp
	src/PluginLoader/Filesystem-common.cpp
	src/PluginLoader/StringUtil.cpp

	include/PluginLoader/PluginInterface.h
	src/PluginLoader/BasicPluginInterface.h
	src/PluginLoader/CodeAllocator.h
	src/PluginLoader/CodeInjectionStream.h
	src/PluginLoader/FuncInterceptor.h
	src/PluginLoader/TrampolineGenerator.h
	src/PluginLoader/Filesystem.h
	src/PluginLoader/StringUtil.h
)
if (UNIX)
	set (PLUGINLOADER_SRC ${PLUGINLOADER_SRC}
		src/PluginLoader/unix/LDE64-as.s
		src/PluginLoader/unix/main-unix.cpp
		src/PluginLoader/unix/Memory-unix.cpp
		src/PluginLoader/unix/SharedObject-unix.cpp
		src/PluginLoader/unix/Filesystem-unix.cpp
	)
elseif (WIN32)
	set (PLUGINLOADER_SRC ${PLUGINLOADER_SRC}
		src/PluginLoader/win32/LDE64-masm.asm
		src/PluginLoader/win32/main-win32.cpp
		src/PluginLoader/win32/Memory-win32.cpp
		src/PluginLoader/win32/SharedObject-win32.cpp
		src/PluginLoader/win32/Filesystem-win32.cpp
	)
endif ()
add_library (PluginLoader SHARED ${PLUGINLOADER_SRC})
if (MSVC)
	set_property (TARGET PluginLoader APPEND PROPERTY LINK_FLAGS "/DEF:\"${PROJECT_SOURCE_DIR}/src/PluginLoader/win32/PluginLoader.def\"")
endif ()

# TorqueLib
set (TORQUELIB_SRC
	src/TorqueLib/TorqueLib.cpp
	src/TorqueLib/math/mAngAxis.cpp
	src/TorqueLib/math/mathUtils.cpp
	src/TorqueLib/math/mBox.cpp
	src/TorqueLib/math/mEase.cpp
	src/TorqueLib/math/mMath_C.cpp
	src/TorqueLib/math/mMathAMD.cpp
	src/TorqueLib/math/mMathSSE.cpp
	src/TorqueLib/math/mMatrix.cpp
	src/TorqueLib/math/mOrientedBox.cpp
	src/TorqueLib/math/mPlane.cpp
	src/TorqueLib/math/mPlaneTransformer.cpp
	src/TorqueLib/math/mPoint.cpp
	src/TorqueLib/math/mQuat.cpp
	src/TorqueLib/math/mRandom.cpp
	src/TorqueLib/math/mRect.cpp
	src/TorqueLib/math/mSolver.cpp
	src/TorqueLib/math/mSphere.cpp
	src/TorqueLib/math/util/quadTransforms.cpp

	include/TorqueLib/TGE.h
	include/TorqueLib/QuickOverride.h
	include/TorqueLib/platform/platform.h
	include/TorqueLib/platform/platformAssert.h
	include/TorqueLib/math/mAngAxis.h
	include/TorqueLib/math/mathUtils.h
	include/TorqueLib/math/mBox.h
	include/TorqueLib/math/mBoxBase.h
	include/TorqueLib/math/mConstants.h
	include/TorqueLib/math/mEase.h
	include/TorqueLib/math/mMath.h
	include/TorqueLib/math/mMathFn.h
	include/TorqueLib/math/mMatrix.h
	include/TorqueLib/math/mOrientedBox.h
	include/TorqueLib/math/mPlane.h
	include/TorqueLib/math/mPlaneSet.h
	include/TorqueLib/math/mPlaneTransformer.h
	include/TorqueLib/math/mPoint2.h
	include/TorqueLib/math/mPoint3.h
	include/TorqueLib/math/mPoint4.h
	include/TorqueLib/math/mQuat.h
	include/TorqueLib/math/mRandom.h
	include/TorqueLib/math/mRandomDeck.h
	include/TorqueLib/math/mRandomSet.h
	include/TorqueLib/math/mRect.h
	include/TorqueLib/math/mSilhouetteExtractor.h
	include/TorqueLib/math/mSphere.h
	include/TorqueLib/math/mTransform.h
	include/TorqueLib/math/util/quadTransforms.h
	include/TorqueLib/util/tVector.h
)
if (WIN32)
	set (TORQUELIB_SRC ${TORQUELIB_SRC}
		include/TorqueLib/win32/Addresses-win32.h
		include/TorqueLib/win32/InterfaceMacros-win32.h
	)
elseif (APPLE)
	set (TORQUELIB_SRC ${TORQUELIB_SRC}
		include/TorqueLib/osx/Addresses-osx.h
		include/TorqueLib/linux/InterfaceMacros-linux.h
	)
elseif (UNIX)
	set (TORQUELIB_SRC ${TORQUELIB_SRC}
		include/TorqueLib/linux/Addresses-linux.h
		include/TorqueLib/linux/InterfaceMacros-linux.h
	)
endif()
add_library (TorqueLib SHARED ${TORQUELIB_SRC})
set_property (TARGET TorqueLib APPEND PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include/TorqueLib/math ${PROJECT_SOURCE_DIR}/include/TorqueLib/math/util)

# TestPlugin
add_library (TestPlugin MODULE plugins/TestPlugin/TestPlugin.cpp)
target_link_libraries (TestPlugin TorqueLib)

# ExternalConsole
add_library (ExternalConsole MODULE plugins/ExternalConsole/ExternalConsole.cpp)
target_link_libraries (ExternalConsole TorqueLib)

# RandomPowerupsDemo
add_library (RandomPowerupsDemo MODULE plugins/RandomPowerupsDemo/RandomPowerupsDemo.cpp)
target_link_libraries (RandomPowerupsDemo TorqueLib)

# MovingPlatformsFix
add_library (MovingPlatformsFix MODULE plugins/MovingPlatformsFix/MovingPlatformsFix.cpp)
target_link_libraries (MovingPlatformsFix TorqueLib)

# MultiplayerFix (Mac, Linux)
if (UNIX)
	add_library (MultiplayerFix MODULE plugins/MultiplayerFix/MultiplayerFix.cpp)
	target_link_libraries (MultiplayerFix TorqueLib)
endif ()

# FrameRateUnlock
set (FRAMERATEUNLOCK_SRC
	plugins/FrameRateUnlock/FrameRateUnlock.cpp
	plugins/FrameRateUnlock/GameTimer.hpp
)
if (WIN32)
	set (FRAMERATEUNLOCK_SRC ${FRAMERATEUNLOCK_SRC}
		plugins/FrameRateUnlock/win32/HighPerformanceTimer-win32.cpp
		plugins/FrameRateUnlock/win32/HighPerformanceTimer-win32.hpp
		plugins/FrameRateUnlock/win32/MultimediaTimer-win32.cpp
		plugins/FrameRateUnlock/win32/MultimediaTimer-win32.hpp
	)
elseif (APPLE)
	set (FRAMERATEUNLOCK_SRC ${FRAMERATEUNLOCK_SRC}
		plugins/FrameRateUnlock/osx/MachTimer-osx.cpp
		plugins/FrameRateUnlock/osx/MachTimer-osx.hpp
	)
elseif (UNIX)
	set (FRAMERATEUNLOCK_SRC ${FRAMERATEUNLOCK_SRC}
		plugins/FrameRateUnlock/linux/MonotonicTimer-linux.cpp
		plugins/FrameRateUnlock/linux/MonotonicTimer-linux.hpp
	)
endif ()
add_library (FrameRateUnlock MODULE ${FRAMERATEUNLOCK_SRC})
target_link_libraries (FrameRateUnlock TorqueLib)
if (WIN32)
	target_link_libraries (FrameRateUnlock winmm)
endif ()

# FakeIgnition (Linux)
if (UNIX AND NOT APPLE)
	add_library (FakeIgnition MODULE plugins/FakeIgnition/FakeIgnition.cpp)
	target_link_libraries (FakeIgnition TorqueLib)
endif ()

# Remove the "lib" prefix from libraries
set_target_properties (PluginLoader TorqueLib TestPlugin ExternalConsole RandomPowerupsDemo MovingPlatformsFix FrameRateUnlock PROPERTIES PREFIX "")
if (WIN32)
	set_target_properties (FakeVorbis PROPERTIES PREFIX "")
elseif (UNIX)
	set_target_properties (MultiplayerFix PROPERTIES PREFIX "")
	if (NOT APPLE)
		set_target_properties (FakeIgnition PROPERTIES PREFIX "")
	endif ()
endif ()

# Install rules 
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()
install (TARGETS MBExtender DESTINATION bin)
install (TARGETS PluginLoader RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
install (TARGETS TorqueLib RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
install (TARGETS TestPlugin ExternalConsole RandomPowerupsDemo MovingPlatformsFix FrameRateUnlock DESTINATION bin/plugins)
if (WIN32)
	install (TARGETS FakeVorbis RUNTIME DESTINATION bin)
elseif (UNIX)
	install (TARGETS MultiplayerFix DESTINATION bin/plugins)
	if (NOT APPLE)
		install (TARGETS FakeIgnition DESTINATION bin/plugins)
	endif ()
endif ()
install (DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
