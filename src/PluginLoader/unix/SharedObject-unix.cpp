#include "../SharedObject.h"
#include <dlfcn.h>

const char *SharedObject::DefaultExtension = ".so";

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
	handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
	return (handle != nullptr);
}

bool SharedObject::loaded() const
{
	return (handle != nullptr);
}

bool SharedObject::unload()
{
	if (handle && dlclose(handle) == 0)
	{
		handle = nullptr;
		return true;
	}
	return false;
}

void *SharedObject::getSymbol(const char *name) const
{
	if (handle)
		return dlsym(handle, name);
	return nullptr;
}
