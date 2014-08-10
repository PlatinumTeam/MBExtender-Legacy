#pragma once

namespace LDE
{
	/// <summary>
	/// Address sizes supported by LDE64.
	/// </summary>
	enum LDEAddrSize
	{
		Bits32 = 0,
		Bits64 = 64
	};

	/// <summary>
	/// Computes the length of an instruction at an address.
	/// </summary>
	/// <param name="address">The address of the instruction to decode.</param>
	/// <param name="size">The address size to decode the instruction with.</param>
	extern "C" int __stdcall LDE(void *address, LDEAddrSize size);
}