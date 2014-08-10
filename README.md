MBExtender
==========

Plugin system for Marble Blast Gold which allows developers to add additional functionality to the game engine through writing plugins in native C++ code. Currently only supports the Windows version of the game.

Compiling
---------

Visual Studio 2013 is required to compile. No external dependencies are necessary.

Running
-------

Copy MBExtender.exe, PluginLoader.dll, and TorqueMath.dll to the same folder as your marbleblast.exe file. Create a "plugins" folder and copy any plugin DLL files there. Run the game using MBExtender.exe.

Sample Plugins
--------------

+ *ConsoleSwag* - Creates an external console window for the game
+ *RandomPowerupsDemo* - Causes powerups built into the game engine to have random effects when used
+ *TestPlugin* - Demonstration of how to create a simple plugin

Creating a New Plugin
---------------------

*TODO: fill this in with a basic tutorial or something*
