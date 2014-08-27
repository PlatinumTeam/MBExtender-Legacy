#ifndef TORQUELIB_INTERFACEMACROS_WIN32_H
#define TORQUELIB_INTERFACEMACROS_WIN32_H

// Utility macros
#define EXPAND(...) __VA_ARGS__

// Macro for quickly declaring a function pointer that points to a static address
#define FN(rettype, name, args, addr) static const auto name = (rettype (*) args)addr

// Macros for declaring overloaded function pointers
//
// Usage example:
// OVERLOAD_PTR {
//     OVERLOAD_FN(int, (int x, int y), 0x12345);
//     OVERLOAD_FN(int, (int z), 0x23456);
// } foo;
#define OVERLOAD_PTR static struct

// Huge hack to make this work with Intellisense
// If Intellisense is parsing the file, we show it that a () operator exists so that it can help with parameters
// However, when the compiler actually reads it, it'll only see a typecast operator
#ifndef __INTELLISENSE__
// This is what the compiler will see
#define OVERLOAD_FN(rettype, args, addr)                \
	private:                                            \
		typedef rettype (*t_##addr##_) args;            \
	public:                                             \
		inline operator t_##addr##_ () const            \
		{                                               \
			return reinterpret_cast<t_##addr##_>(addr); \
		}
#else
// This is what intellisense will see
#define OVERLOAD_FN(rettype, args, addr)                \
	private:                                            \
		typedef rettype (*t_##addr##_) args;            \
	public:                                             \
		inline rettype operator () args const           \
		{                                               \
		}                                               \
		inline operator t_##addr##_() const             \
		{                                               \
			return reinterpret_cast<t_##addr##_>(addr); \
		}
#endif

// Defines the offset of a class's vtable pointer. Required in order to use VIRTFN and VIRTFNSIMP.	
#define VTABLE(offset) \
	uint32_t z_vtableLookup_(int index) \
	{ \
		return (*reinterpret_cast<uint32_t**>(reinterpret_cast<uint32_t>(this) + offset))[index]; \
	}
		
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
#define UNKVIRT(x) virtual void z_unk##x##_() = 0

// Defines an undefined pure virtual function
#define UNDEFVIRT(name) virtual void z_undef_##name##_() = 0

// Defines a "raw" member function pointer
#define RAWMEMBERFN(clazz, rettype, name, args, addr) static const auto name = (rettype (__thiscall *) (EXPAND(clazz *thisobj, EXPAND args)))addr

// Same as RAWMEMBERFN, but defines a "simple" pointer with no arguments
#define RAWMEMBERFNSIMP(clazz, rettype, name, addr) static const auto name = (rettype (__thiscall *) (clazz *thisobj))addr

// Defines a wrapper which calls a member function on a class
// Yes, argument names need to be repeated, but there's not much I can do about that AFAIK
#define MEMBERFN(rettype, name, args, argnames, addr)                                                       \
	rettype name args                                                                                       \
	{                                                                                                       \
		return ((rettype (__thiscall *) (EXPAND(void*, EXPAND args)))addr) (EXPAND(this, EXPAND argnames)); \
	}

// Same as MEMBERFN, but defines a "simple" wrapper with no arguments
#define MEMBERFNSIMP(rettype, name, addr)                    \
	rettype name()                                           \
	{                                                        \
		return ((rettype (__thiscall *) (void*))addr)(this); \
	}
	
// Defines a destructor wrapper
#define MEMBERDTOR(name, addr)                     \
	name()                                         \
	{                                              \
		((void (__thiscall *) (void*))addr)(this); \
	}

// Defines a global variable
#define GLOBALVAR(type, name, addr) static type &name = *(type*)addr

// Defines a function which uses the thiscall convention
#define THISFN(rettype, name, args)                                       \
	rettype __fastcall z_thisfn_impl_##name (EXPAND(void*, EXPAND args)); \
	namespace                                                             \
	{                                                                     \
		__declspec(naked) void z_thisfn_jmp_##name args                   \
		{                                                                 \
			__asm mov edx, ecx                                            \
			__asm xor ecx, ecx                                            \
			__asm jmp z_thisfn_impl_##name                                \
		}                                                                 \
	}                                                                     \
	typedef rettype (__thiscall* z_thisfn_ptr_##name) args; \
	const z_thisfn_ptr_##name name = (z_thisfn_ptr_##name) z_thisfn_jmp_##name;  \
	rettype __fastcall z_thisfn_impl_##name (EXPAND(void*, EXPAND args))

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
