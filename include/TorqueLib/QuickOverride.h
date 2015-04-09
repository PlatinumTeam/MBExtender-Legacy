#ifndef QUICKOVERRIDE_H
#define QUICKOVERRIDE_H

#include "TGE.h"

namespace TorqueLib
{
	struct OverrideRequest
	{
		void **originalFunctionPtr;
		void *newFunction;
		OverrideRequest *nextOverride;
	};

	extern DLLSPEC OverrideRequest *requestList; // Linked list of OverrideRequest structures

	// Only instantiate from the global scope!
	template<class T>
	class OverrideGenerator
	{
	public:
		OverrideGenerator(T *originalFunctionPtr, T newFunction)
		{
			// Fill out our OverrideRequest and push it onto the linked list
			request.originalFunctionPtr = reinterpret_cast<void**>(originalFunctionPtr);
			request.newFunction = reinterpret_cast<void*>(newFunction);
			request.nextOverride = requestList;
			requestList = &request;
		}

	private:
		OverrideRequest request;
	};
}

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define MAKEUNIQUE(x) TOKENPASTE2(x, __LINE__)

#define TorqueOverride(rettype, func, args, originalName) \
	typedef rettype (* MAKEUNIQUE(z_torqueOverride_ptr)) args; \
	static rettype MAKEUNIQUE(z_torqueOverride) args; \
	static MAKEUNIQUE(z_torqueOverride_ptr) originalName = TGE:: func; \
	static TorqueLib::OverrideGenerator<MAKEUNIQUE(z_torqueOverride_ptr)> MAKEUNIQUE(z_torqueOverrideGen) (&originalName, MAKEUNIQUE(z_torqueOverride)); \
	static rettype MAKEUNIQUE(z_torqueOverride) args

#define TorqueOverrideFastcall(rettype, func, args, originalName) \
	typedef rettype (__fastcall* MAKEUNIQUE(z_torqueOverride_ptr)) args; \
	static __fastcall rettype MAKEUNIQUE(z_torqueOverride) args; \
	static MAKEUNIQUE(z_torqueOverride_ptr) originalName = TGE:: func; \
	static TorqueLib::OverrideGenerator<MAKEUNIQUE(z_torqueOverride_ptr)> MAKEUNIQUE(z_torqueOverrideGen) (&originalName, MAKEUNIQUE(z_torqueOverride)); \
	static rettype MAKEUNIQUE(z_torqueOverride) args

#define THISFN2(rettype, name, args) THISFN(rettype, name, args)

#ifdef _WIN32
#define TorqueOverrideMember(rettype, func, args, originalName) \
	typedef rettype (__thiscall* MAKEUNIQUE(z_torqueOverride_ptr)) args; \
	extern const MAKEUNIQUE(z_torqueOverride_ptr) MAKEUNIQUE(z_torqueOverride); \
	static MAKEUNIQUE(z_torqueOverride_ptr) originalName = TGE::Members:: func; \
	static TorqueLib::OverrideGenerator<MAKEUNIQUE(z_torqueOverride_ptr)> MAKEUNIQUE(z_torqueOverrideGen) (&originalName, MAKEUNIQUE(z_torqueOverride)); \
	THISFN2(rettype, MAKEUNIQUE(z_torqueOverride), args)
#else
#define TorqueOverrideMember(rettype, func, args, originalName) \
	typedef rettype (* MAKEUNIQUE(z_torqueOverride_ptr)) args; \
	rettype MAKEUNIQUE(z_torqueOverride) args; \
	static MAKEUNIQUE(z_torqueOverride_ptr) originalName = TGE::Members:: func; \
	static TorqueLib::OverrideGenerator<MAKEUNIQUE(z_torqueOverride_ptr)> MAKEUNIQUE(z_torqueOverrideGen) (&originalName, MAKEUNIQUE(z_torqueOverride)); \
	THISFN2(rettype, MAKEUNIQUE(z_torqueOverride), args)
#endif // _WIN32

#endif // QUICKOVERRIDE_H
