#include "../SharedObject.h"
#include <Windows.h>

const char *SharedObject::DefaultExtension = ".dll";

SharedObject::SharedObject()
	: handle(nullptr)
{
}

SharedObject::SharedObject(const char *path)
	: handle(nullptr)
{
	load(path);
}

SharedObject::~SharedObject()
{
	unload();
}

bool SharedObject::load(const char *path)
{
	unload();
	handle = LoadLibrary(path);
	return (handle != nullptr);
}

bool SharedObject::loaded() const
{
	return (handle != nullptr);
}

bool SharedObject::unload()
{
	if (handle && CloseHandle(static_cast<HMODULE>(handle)) != 0)
	{
		handle = nullptr;
		return true;
	}
	return false;
}

void *SharedObject::getSymbol(const char *name) const
{
	if (handle)
		return GetProcAddress(static_cast<HMODULE>(handle), name);
	return nullptr;
}
