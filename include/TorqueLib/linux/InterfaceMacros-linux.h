#ifndef TORQUELIB_INTERFACEMACROS_LINUX_H
#define TORQUELIB_INTERFACEMACROS_LINUX_H

// Utility macros
#define EXPAND(...) __VA_ARGS__

// Macro for quickly declaring a function pointer that points to a static address
#define FN(rettype, name, args, addr) namespace { rettype (*const name)args = (rettype (*) args)addr; }

// Macros for declaring overloaded function pointers
//
// Usage example:
// OVERLOAD_PTR {
//     OVERLOAD_FN(int, (int x, int y), 0x12345);
//     OVERLOAD_FN(int, (int z), 0x23456);
// } foo;
#define OVERLOAD_PTR static struct

#define OVERLOAD_FN(rettype, args, addr)                \
	private:                                            \
		typedef rettype (*t_##addr##_) args;            \
	public:                                             \
		inline operator t_##addr##_ () const            \
		{                                               \
			return reinterpret_cast<t_##addr##_>(addr); \
		}
	
// Defines the offset of a class's vtable pointer. Required in order to use VIRTFN and VIRTFNSIMP.	
#define VTABLE(offset) \
	uint32_t z_vtableLookup_(int index) \
	{ \
		return (*reinterpret_cast<uint32_t**>(reinterpret_cast<uint32_t>(this) + offset))[index + 2]; \
	} \
	virtual void z_foo_() = 0
		
// Defines a virtual function wrapper
#define VIRTFN(rettype, name, args, argnames, index) \
	MEMBERFN(rettype, name, args, argnames, z_vtableLookup_(index))

// Same as VIRTFN, but defines a "simple" virtual function with no arguments
#define VIRTFNSIMP(rettype, name, index) \
	MEMBERFNSIMP(rettype, name, z_vtableLookup_(index))
	
// Defines a virtual destructor wrapper
#define VIRTDTOR(name, index) \
	MEMBERDTOR(name, z_vtableLookup_(index))

// Defines an unnamed pure virtual function
#define UNKVIRT(x)

// Defines an undefined pure virtual function
#define UNDEFVIRT(name)
	
// Defines a "raw" member function pointer
#define RAWMEMBERFN(clazz, rettype, name, args, addr) namespace { const auto name = (rettype (*) (EXPAND(clazz *thisobj, EXPAND args)))addr; }

// Same as RAWMEMBERFN, but defines a "simple" pointer with no arguments
#define RAWMEMBERFNSIMP(clazz, rettype, name, addr) namespace { const auto name = (rettype (*) (clazz *thisobj))addr; }

// Defines a wrapper which calls a member function on a class
// Yes, argument names need to be repeated, but there's not much I can do about that AFAIK
#define MEMBERFN(rettype, name, args, argnames, addr)                                              \
	rettype name args                                                                              \
	{                                                                                              \
		return ((rettype (*) (EXPAND(void*, EXPAND args)))(addr)) (EXPAND(this, EXPAND argnames)); \
	}

// Same as MEMBERFN, but defines a "simple" wrapper with no arguments
#define MEMBERFNSIMP(rettype, name, addr)           \
	rettype name()                                  \
	{                                               \
		return ((rettype (*) (void*))(addr))(this); \
	}
	
// Defines a destructor wrapper
#define MEMBERDTOR(name, addr)            \
	name()                                \
	{                                     \
		((void (*) (void*))(addr))(this); \
	}

// Defines a global variable
#define GLOBALVAR(type, name, addr) namespace { type &name = *(type*)addr; }

// Defines a function which uses the thiscall convention
#define THISFN(rettype, name, args) \
	rettype name args

// Defines a getter for a class field
#define GETTERFN(type, name, offset)                                             \
	type name()                                                                  \
	{                                                                            \
		return *reinterpret_cast<type*>(reinterpret_cast<char*>(this) + offset); \
	}

// Defines a setter for a class field
#define SETTERFN(type, name, offset)                                                 \
	void name(type newValue)                                                         \
	{                                                                                \
		*reinterpret_cast<type*>(reinterpret_cast<char*>(this) + offset) = newValue; \
	}

#endif // TORQUELIB_INTERFACEMACROS_LINUX_H
