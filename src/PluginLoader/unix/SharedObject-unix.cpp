#include "../SharedObject.h"
#include <dlfcn.h>
#include <stdio.h>

const char *SharedObject::DefaultExtension = ".so";

SharedObject::SharedObject()
	: handle(NULL)
{
}

SharedObject::SharedObject(const char *path)
	: handle(NULL)
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
	if (!handle)
	{
		fprintf(stderr, "Unable to load shared object %s: %s\n", path, dlerror());
		return false;
	}
	return true;
}

bool SharedObject::loaded() const
{
	return (handle != NULL);
}

bool SharedObject::unload()
{
	if (handle && dlclose(handle) == 0)
	{
		handle = NULL;
		return true;
	}
	return false;
}

void *SharedObject::getSymbol(const char *name) const
{
	if (handle)
		return dlsym(handle, name);
	return NULL;
}
