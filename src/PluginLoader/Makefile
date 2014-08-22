CPP=g++
CPPFLAGS=-c -m32 --std=c++11 -fpic -DIN_PLUGIN_LOADER -g

all: PluginLoader.so

clean:
	rm -f *.o
	rm -f unix/*.o
	rm -f PluginLoader.so

PluginLoader.so: CodeAllocator.o CodeInjectionStream.o FuncInterceptor.o unix/LDE64-as.o unix/Memory-unix.o PluginLoader.o unix/SharedObject-unix.o TrampolineGenerator.o Win32PluginInterface.o unix/main-unix.o
	$(CPP) -m32 -shared -L/usr/lib32 -lboost_filesystem -lboost_system -ldl -o PluginLoader.so CodeAllocator.o CodeInjectionStream.o FuncInterceptor.o unix/LDE64-as.o unix/Memory-unix.o PluginLoader.o unix/SharedObject-unix.o TrampolineGenerator.o Win32PluginInterface.o unix/main-unix.o

CodeAllocator.o: CodeAllocator.cpp
	$(CPP) $(CPPFLAGS) -o CodeAllocator.o CodeAllocator.cpp

CodeInjectionStream.o: CodeInjectionStream.cpp
	$(CPP) $(CPPFLAGS) -o CodeInjectionStream.o CodeInjectionStream.cpp
	
FuncInterceptor.o: FuncInterceptor.cpp
	$(CPP) $(CPPFLAGS) -o FuncInterceptor.o FuncInterceptor.cpp
	
unix/LDE64-as.o: unix/LDE64-as.s
	as --32 -c -o unix/LDE64-as.o unix/LDE64-as.s
	
unix/Memory-unix.o: unix/Memory-unix.cpp
	$(CPP) $(CPPFLAGS) -o unix/Memory-unix.o unix/Memory-unix.cpp
	
PluginLoader.o: PluginLoader.cpp
	$(CPP) $(CPPFLAGS) -o PluginLoader.o PluginLoader.cpp
	
unix/SharedObject-unix.o: unix/SharedObject-unix.cpp
	$(CPP) $(CPPFLAGS) -o unix/SharedObject-unix.o unix/SharedObject-unix.cpp
	
TrampolineGenerator.o: TrampolineGenerator.cpp
	$(CPP) $(CPPFLAGS) -o TrampolineGenerator.o TrampolineGenerator.cpp
	
Win32PluginInterface.o: Win32PluginInterface.cpp
	$(CPP) $(CPPFLAGS) -o Win32PluginInterface.o Win32PluginInterface.cpp
	
unix/main-unix.o: unix/main-unix.cpp
	$(CPP) $(CPPFLAGS) -o unix/main-unix.o unix/main-unix.cpp
