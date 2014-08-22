MBExtender
==========

Plugin system for Marble Blast Gold which allows developers to add additional functionality to the game engine through writing plugins in native C++ code. Currently supports the Windows and Linux versions of the game.

On Linux, a plugin (FakeIgnition) is included which disables the game's ignition system so that a valid registration key is not required in order to play the game. It is not possible to legally purchase the game anymore.

Compiling
---------

A 32-bit version of [Boost](http://www.boost.org/) (specifically, [Boost.Filesystem](http://www.boost.org/doc/libs/1_56_0/libs/filesystem/doc/index.htm)) is required to compile MBExtender on all platforms. Ensure that your compiler has access to the Boost includes and libraries.

#### Windows ####
Visual Studio 2013 is required to compile MBExtender on Windows. Open MBExtender.sln, select either Debug or Release mode, and then click Build -> Build Solution. Binaries will be found in a folder in the solution directory corresponding to the configuration you chose (either Debug or Release). Plugins will be in the "plugins" folder inside of it.

#### Linux ####
A standard G++ installation supporting -std=c++11 is required to compile MBExtender on Linux. Open a shell and run the following commands:
```
$ ./configure "--prefix=$PWD/release"
$ make
$ make install
```
If all goes well, MBExtender's files will be installed to a "release" folder in the current directory. Binaries can be found in the "bin" directory inside of it.

Running
-------

#### Windows ####
There are two methods of activating the plugin loader on Windows.

The preferred method is to use a proxy vorbis.dll (FakeVorbis), because it does not require administrator access in order to run the game. Open the folder with your marbleblast.exe file in it. Find vorbis.dll and rename it to "vorbis-real.dll". Then, copy vorbis.dll (built as part of MBExtender), PluginLoader.dll, and TorqueLib.dll to that folder, and place any plugin DLLs inside of a "plugins" folder.

An external launcher similar to the one used on Linux is also available, but it requires administrator access. Copy MBExtender.exe, PluginLoader.dll, and TorqueLib.dll to the same folder as your marbleblast.exe file. Create a "plugins" folder and copy any plugin DLL files there. Run the game using MBExtender.exe.

#### Linux ####
Copy the contents of your "bin" folder (see Compiling above) to the same folder as marbleblastgold.bin. Run the game using the MBExtender executable.

Sample Plugins
--------------

+ *MovingPlatformsFix* - Fixes moving platforms in multiplayer
+ *ExternalConsole* - Creates an external console window for the game
+ *RandomPowerupsDemo* - Causes powerups built into the game engine to have random effects when used
+ *TestPlugin* - Demonstration of how to create a simple plugin
+ *FakeIgnition* - (Linux only) Disables the game's ignition system

Creating a New Plugin
---------------------

*TODO: fill this in with a basic tutorial or something*
