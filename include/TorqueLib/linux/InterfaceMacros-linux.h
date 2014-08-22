#ifndef TORQUELIB_INTERFACEMACROS_UNIX_H
#define TORQUELIB_INTERFACEMACROS_UNIX_H

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

// Defines an unnamed pure virtual function
#define UNKVIRT(x) virtual void z_unk##x##_() = 0

// Defines an undefined pure virtual function
#define UNDEFVIRT(name) virtual void z_undef_##name##_() = 0

// RTTI info
#define RTTI_INFO                           \
	virtual void z_rtti_offsetToTop_() = 0; \
	virtual void z_rtti_typeInfoPtr_() = 0
	
// Defines a "raw" member function pointer
#define RAWMEMBERFN(clazz, rettype, name, args, addr) namespace { const auto name = (rettype (*) (EXPAND(clazz *thisobj, EXPAND args)))addr; }

// Same as RAWMEMBERFN, but defines a "simple" pointer with no arguments
#define RAWMEMBERFNSIMP(clazz, rettype, name, addr) namespace { const auto name = (rettype (*) (clazz *thisobj))addr; }

// Defines a wrapper which calls a member function on a class
// Yes, argument names need to be repeated, but there's not much I can do about that AFAIK
#define MEMBERFN(rettype, name, args, argnames, addr)                                            \
	rettype name args                                                                            \
	{                                                                                            \
		return ((rettype (*) (EXPAND(void*, EXPAND args)))addr) (EXPAND(this, EXPAND argnames)); \
	}

// Same as MEMBERFN, but defines a "simple" wrapper with no arguments
#define MEMBERFNSIMP(rettype, name, addr)                    \
	rettype name()                                           \
	{                                                        \
		return ((rettype (*) (void*))addr)(this); \
	}

// Defines a global variable
#define GLOBALVAR(type, name, addr) namespace { type &name = *(type*)addr; }

// Defines a function which uses the thiscall convention
#define THISFN(rettype, name, args)          \
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

#endif // TORQUELIB_INTERFACEMACROS_WIN32_H
