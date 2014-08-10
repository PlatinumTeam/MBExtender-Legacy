#include "CodeAllocator.h"
#include <Windows.h>

namespace CodeInjection
{
	CodeAllocator::CodeAllocator()
		: ptr(nullptr), sizeRemaining(0)
	{
	}

	CodeAllocator::~CodeAllocator()
	{
		releaseAllBuffers();
	}

	/// <summary>
	/// Allocates a block of code.
	/// </summary>
	/// <param name="size">The size of the block to allocate.</param>
	/// <returns>The allocated block if successful, or <c>nullptr</c> on failure.</returns>
	void* CodeAllocator::allocate(size_t size)
	{
		if (!ensureAvailable(size))
			return nullptr;

		// Simple push-back-the-pointer allocation
		uint8_t *result = ptr;
		ptr += size;
		sizeRemaining -= size;
		return result;
	}

	/// <summary>
	/// Ensures that a number of bytes is available in the buffer.
	/// If the buffer is too small, a new one will be allocated.
	/// </summary>
	/// <param name="size">The number of bytes requested.</param>
	/// <returns><c>true</c> on success.</returns>
	bool CodeAllocator::ensureAvailable(size_t size)
	{
		if (size > sizeRemaining)
			return newBuffer(size);
		return true;
	}

	/// <summary>
	/// Allocates a new buffer.
	/// </summary>
	/// <param name="minSize">The minimum size that the buffer should be.</param>
	/// <returns><c>true</c> on success.</returns>
	bool CodeAllocator::newBuffer(size_t minSize)
	{
		// Allocate an executable page for the buffer
		void *buffer = VirtualAlloc(nullptr, minSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (buffer == nullptr)
			return false;

		// Query the actual size of it
		MEMORY_BASIC_INFORMATION info;
		if (VirtualQuery(buffer, &info, sizeof(MEMORY_BASIC_INFORMATION)) != sizeof(MEMORY_BASIC_INFORMATION))
		{
			releaseBuffer(buffer);
			return false;
		}

		// Set the current pointer, set the size remaining, and save the buffer so it can be freed
		ptr = static_cast<uint8_t*>(buffer);
		sizeRemaining = info.RegionSize;
		buffers.push(ptr);
		return true;
	}

	/// <summary>
	/// Frees the memory occupied by a buffer.
	/// It must have been allocated with <see cref="newBuffer"/>.
	/// </summary>
	/// <param name="buffer">The buffer to release.</param>
	void CodeAllocator::releaseBuffer(void *buffer)
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
	}

	/// <summary>
	/// Frees the memory occupied by all allocated buffers.
	/// </summary>
	void CodeAllocator::releaseAllBuffers()
	{
		while (!buffers.empty())
		{
			releaseBuffer(buffers.top());
			buffers.pop();
		}
		ptr = nullptr;
		sizeRemaining = 0;
	}
}