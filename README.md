MBExtender
==========

Plugin system for Marble Blast Gold which allows developers to add additional functionality to the game engine by writing plugins in native C++ code. The Windows, Mac, and Linux versions of the game are all supported. Plugins share a common interface on each platform and can be compiled for other systems with ease.

Also included with MBExtender are some plugins which improve the game's playability:
+ On all platforms, a FrameRateUnlock plugin is included which replaces the game loop timing code so that the game's frame rate is not limited by the resolution of the system timer.
+ On all platforms, a MovingPlatformsFix plugin is included which fixes moving platforms syncing incorrectly in multiplayer.
+ On Mac and Linux, a MultiplayerFix plugin is included which enables Mac and Linux players to connect to Windows players without the game crashing.
+ On Linux, a FakeIgnition plugin is included which disables the game's ignition system so that a valid registration key is not required in order to play the game. It is not possible to legally purchase the game anymore.

Compiling
---------

#### Prerequisites ####
MBExtender uses [CMake](http://www.cmake.org/) for its build system. You must install CMake in order to compile MBExtender.

On Windows, Visual Studio 2013 is currently required in order to compile MBExtender.

A 32-bit version of [Boost](http://www.boost.org/) (specifically, [Boost.System](http://www.boost.org/doc/libs/1_57_0/libs/system/doc/index.html) and [Boost.Filesystem](http://www.boost.org/doc/libs/1_56_0/libs/filesystem/doc/index.htm)) is also required. Ensure that your compiler has access to the Boost includes and libraries. You may need the static libraries present on your system in order for CMake to configure the project successfully, but static linking to Boost can be toggled by changing the Boost_USE_STATIC_LIBS configuration option.

If CMake cannot find where Boost is installed, first verify that you have the 32-bit libraries installed and not just the 64-bit ones. If you do have the 32-bit libraries installed, then you will need to set the $BOOST_ROOT environment variable to point to the location where Boost is installed. Library files will be searched for in $BOOST_ROOT/lib, and include files will be searched for in $BOOST_ROOT/boost.

#### Windows ####
Open CMake, click "Browse Source," and select your MBExtender source folder. Also specify a separate build folder if one is not filled in automatically for you. Click "Configure" and select "Visual Studio 12 2013" as the target. If all goes well, then click Generate to create Visual Studio projects in the build directory.

After projects have been created, open MBExtender.sln in the build directory you chose. Select the configuration you want to use (Debug/Release) and then click "Build Solution" in the build menu. If compilation is successful, right-click the "INSTALL" project in Solution Explorer and build it to install the MBExtender binaries. By default, MBExtender will be installed to an "install" subfolder of your build directory.

#### Mac OS X ####
Open CMake, click "Browse Source," and select your MBExtender source folder. Also specify a separate build folder if one is not filled in automatically for you. Click "Configure" and select either "Unix Makefiles" or "Xcode" as the target. If all goes well, then click Generate to create a project or makefile in the build directory.

See the Linux section below for information on building from Unix Makefiles.

**Note: if you choose to create an Xcode project, ensure that the build target is set to 32-bit. Otherwise, build errors may occur or an incorrect binary will be built.**

By default, MBExtender will be installed to an "install" subfolder of your build directory.

#### Linux ####
Open CMake, click "Browse Source," and select your MBExtender source folder. Also specify a separate build folder if one is not filled in automatically for you. Click "Configure" and select "Unix Makefiles" as the target. If all goes well, then click Generate to create a project or makefile in the build directory.

CMake can also be used from a terminal:
```
$ mkdir build
$ cd build
$ cmake -G "Unix Makefiles" ...
```

Finally, to build and install MBExtender, run the following commands from inside the build directory:
```
$ make
$ make install
```

By default, MBExtender will be installed to an "install" subfolder of your build directory.

Running
-------

#### Windows ####
There are two methods of activating the plugin loader on Windows.

The preferred method is to use a proxy vorbis.dll (FakeVorbis), because it does not require administrator access in order to run the game. Open the folder with your marbleblast.exe file in it. Find vorbis.dll and rename it to "vorbis-real.dll". Then, copy FakeVorbis.dll (built as part of MBExtender), PluginLoader.dll, and TorqueLib.dll to that folder, and place any plugin DLLs inside of a "plugins" folder. Run the game by simply launching marbleblast.exe.

An external launcher similar to the one used on Mac and Linux is also available, but it requires administrator access. Copy MBExtender.exe, PluginLoader.dll, and TorqueLib.dll to the same folder as your marbleblast.exe file. Create a "plugins" folder and copy any plugin DLL files there. Run the game using MBExtender.exe.

#### Mac OS X ####
PluginLoader.dylib, TorqueLib.dylib, and the plugins folder must be copied to the Contents/MacOS folder inside the game's application package. Edit the game's Info.plist to change the executable that is launched, re-copy the game to your Applications folder if necessary, and launch it normally.

#### Linux ####
Copy the contents of your "bin" folder (see Compiling above) to the same folder as marbleblastgold.bin. Run the game using the MBExtender executable.

Included Plugins
----------------

+ *FrameRateUnlock* - Unlocks the game's frame rate
+ *MovingPlatformsFix* - Fixes moving platforms in multiplayer
+ *MultiplayerFix* - (Mac and Linux only) Fixes cross-platform multiplayer
+ *FakeIgnition* - (Linux only) Disables the game's ignition system
+ *ExternalConsole* - Creates an external console window for the game
+ *RandomPowerupsDemo* - Causes powerups built into the game engine to have random effects when used
+ *TestPlugin* - Used to test various features

Creating a New Plugin
---------------------

*TODO: fill this in with a basic tutorial or something*
