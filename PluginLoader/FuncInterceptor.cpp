#include "PeSectionFinder.h"
#include "FuncInterceptor.h"

// Name of PE section that code resides in
const char *sectionName = ".text";

namespace
{
	// Size of a 32-bit relative jump
	const size_t JumpSize = 5;
}

namespace CodeInjection
{
	FuncInterceptor::FuncInterceptor(CodeAllocator *allocator)
		: stream(nullptr), trampolineGen(allocator)
	{
		CodeInjection::PeSectionFinder pe;
		CodeInjection::PeSectionInfo code;
		if (!pe.findSection(sectionName, &code))
			return;
		stream = new CodeInjection::CodeInjectionStream(code.start, code.virtualSize);
	}

	FuncInterceptor::~FuncInterceptor()
	{
		if (stream != nullptr)
		{
			restoreAll();
			delete stream;
			stream = nullptr;
		}
	}

	/// <summary>
	/// Implementation of <see cref="intercept"/>.
	/// </summary>
	/// <param name="func">The function to intercept.</param>
	/// <param name="newFunc">The new function to redirect callers to.</param>
	/// <returns>A pointer which can be used to call the original function, or <c>nullptr</c> on failure.</returns>
	void *FuncInterceptor::interceptImpl(void *func, void *newFunc)
	{
		if (stream == nullptr || func == nullptr || newFunc == nullptr)
			return nullptr;

		// As an optimization, if the function is a thunk (it consists of only a relative jump), then a trampoline isn't necessary
		stream->seekTo(func);
		void *originalFunc = stream->peekRel32Jump();
		if (originalFunc == nullptr)
		{
			// Not a thunk - create a trampoline
			originalFunc = trampolineGen.createTrampoline(func, JumpSize);
			if (originalFunc == nullptr)
				return nullptr;
		}
		
		// Write a jump to the new function and store the original function pointer
		stream->writeRel32Jump(newFunc);
		originalFunctions[func] = originalFunc;
		return originalFunc;
	}

	/// <summary>
	/// Restores a function to point to its old code.
	/// </summary>
	/// <param name="func">The function to restore.</param>
	/// <param name="oldFunc">The old code pointer.</param>
	void FuncInterceptor::restore(void *func, void *oldFunc)
	{
		if (stream == nullptr)
			return;
		stream->seekTo(func);
		stream->writeRel32Jump(oldFunc);
	}

	/// <summary>
	/// Restores the specified function.
	/// </summary>
	/// <param name="func">The function to restore.</param>
	void FuncInterceptor::restore(void *func)
	{
		if (stream == nullptr)
			return;
		auto it = originalFunctions.find(func);
		if (it == originalFunctions.end())
			return;
		restore(func, it->second);
		originalFunctions.erase(it);
	}

	/// <summary>
	/// Restores all intercepted functions.
	/// </summary>
	void FuncInterceptor::restoreAll()
	{
		for (auto func : originalFunctions)
			restore(func.first, func.second);
		originalFunctions.clear();
	}
}