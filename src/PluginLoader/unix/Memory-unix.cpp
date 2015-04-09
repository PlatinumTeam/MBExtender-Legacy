#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include <unordered_map>
#include "../Memory.h"

namespace Memory
{
	/// <summary>
	/// Allocates a block of readable, writable, and executable memory.
	/// </summary>
	/// <param name="minSize">The minimum amount of data to allocate.</param>
	/// <param name="actualSize">Variable to store the actual allocated size to.</param>
	/// <returns>The pointer to the allocated data if successful, or <c>NULL</c> otherwise.</returns>
	void *allocateCode(size_t minSize, size_t *actualSize)
	{
		long pageSize = sysconf(_SC_PAGESIZE);
		size_t size = (minSize + pageSize - 1) & ~(pageSize - 1); // Round minSize up to a multiple of pageSize
		void *buffer = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if (buffer != MAP_FAILED)
		{
			*actualSize = size;
			return buffer;
		}
		return NULL;
	}
	
	/// <summary>
	/// Frees a block of code allocated with <see cref="allocateCode"/>.
	/// </summary>
	/// <param name="code">Pointer to the code to free. It must have been allocated with <see cref="allocateCode"/>.
	/// <param name="size">Size of the block to free.</param>
	/// <returns><c>true</c> if successful.</returns>
	bool freeCode(void *code, size_t size)
	{
		return (munmap(code, size) == 0);
	}
	
	/// <summary>
	/// Flushes the instruction cache for a block of code.
	/// </summary>
	/// <param name="start">Start of the block of code to flush.</param>
	/// <param name="size">Size of the block of code to flush.</param>
	void flushCode(void *start, size_t size)
	{
		// Do nothing, not necessary on x86
	}
	
	namespace
	{
		// Holds current memory protection state info for addresses
		std::unordered_map<void*, int>* protection;
		const int DefaultProtection = PROT_EXEC | PROT_READ;         // Default memory protection for code
		const int RWXProtection = PROT_EXEC | PROT_READ | PROT_WRITE; // Read+write+exec memory protection for code
		
		void pageAlign(void *ptr, size_t size, void **resultPtr, size_t *resultSize)
		{
			size_t addr = reinterpret_cast<size_t>(ptr);
			long pageSize = sysconf(_SC_PAGESIZE);
			size_t alignedAddr = addr & ~(pageSize - 1);
			*resultSize = (size + addr - alignedAddr + pageSize - 1) & ~(pageSize - 1);
			*resultPtr = reinterpret_cast<void*>(alignedAddr);
		}
	}
	
	/// <summary>
	/// Unprotects a block of code so that it can be written to.
	/// </summary>
	/// <param name="code">Start of the region of code to unprotect.</param>
	/// <param name="size">Size of the region of code to unprotect.</param>
	/// <param name="oldProtection">Variable to receive the old protection state. The meaning of this value is platform-dependant.</param>
	/// <returns><c>true</c> if successful.</returns>
	bool unprotectCode(void *code, size_t size, int *oldProtection)
	{
		void *alignedCode;
		size_t alignedSize;
		pageAlign(code, size, &alignedCode, &alignedSize);
		
		const int newProtection = RWXProtection;
		if (mprotect(alignedCode, alignedSize, newProtection) != 0)
		{
			perror("Unprotection failed");
			return false;
		}
		
		// Look up old protection state in the protection map
		if (!protection)
		{
			protection = new std::unordered_map<void*, int>();
			*oldProtection = DefaultProtection;
		}
		else
		{
			std::unordered_map<void*, int>::const_iterator it = protection->find(alignedCode);
			if (it != protection->end())
				*oldProtection = it->second;
			else
				*oldProtection = DefaultProtection; // No easy way of querying this, so just assume it's default
		}
			
		(*protection)[alignedCode] = newProtection;
		return true;
	}
	
	/// <summary>
	/// Protects a block of code that was previously unprotected with <see cref="unprotectCode"/>.
	/// </summary>
	/// <param name="code">Start of the region of code to protect.</param>
	/// <param name="size">Size of the region of code to protect.</param>
	/// <param name="protection">The oldProtection value received from <see cref="unprotectCode"/>.
	/// <returns><c>true</c> if successful.</returns>
	bool protectCode(void *code, size_t size, int oldProtection)
	{
		void *alignedCode;
		size_t alignedSize;
		pageAlign(code, size, &alignedCode, &alignedSize);
		if (mprotect(alignedCode, alignedSize, oldProtection) != 0)
			return false;
			
		// Update protection state in the protection map
		if (!protection)
			protection = new std::unordered_map<void*, int>();
		if (oldProtection != DefaultProtection)
			(*protection)[alignedCode] = oldProtection;
		else
			protection->erase(alignedCode);
		return true;
	}
}
