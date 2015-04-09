#include "CodeInjectionStream.h"
#include "Memory.h"
#include <cstring>

namespace
{
	const uint8_t Rel32JumpOpcode = 0xE9;
	const uint8_t Rel32CallOpcode = 0xE8;
	const uint8_t NopOpcode = 0x90;
}

namespace CodeInjection
{
	CodeInjectionStream::CodeInjectionStream(void *start, size_t size)
		: start(static_cast<uint8_t*>(start)), currentPtr(static_cast<uint8_t*>(start)), size(size), needsFlush(false), oldProtection(0)
	{
		unprotect();
	}

	CodeInjectionStream::~CodeInjectionStream()
	{
		flush();
		protect();
	}

	/// <summary>
	/// Writes data to the stream at the current position, advancing the stream by the size of the data.
	/// </summary>
	/// <param name="data">The data to write.</param>
	/// <param name="dataSize">The size of the data to write. It must fit.</param>
	void CodeInjectionStream::write(const void *data, size_t dataSize)
	{
		if (isSpaceAvailable(dataSize))
		{
			memcpy(currentPtr, data, dataSize);
			currentPtr += dataSize;
			needsFlush = true;
		}
	}

	/// <summary>
	/// Reads data from the stream at the current position, advancing the stream by the size of the data.
	/// </summary>
	/// <param name="out">The buffer to write data to.</param>
	/// <param name="size">The number of bytes to read.</param>
	/// <returns><c>true</c> if the read was successful.</param>
	bool CodeInjectionStream::read(void *out, size_t size)
	{
		if (!isSpaceAvailable(size))
			return false;
		memcpy(out, currentPtr, size);
		currentPtr += size;
		return true;
	}

	/// <summary>
	/// Reads a relative 32-bit jump or call instruction from the stream at the current position, advancing the stream by the size of the instruction.
	/// </summary>
	/// <returns>The target of the far jump or call if valid, or <c>NULL</c> otherwise.</returns>
	void* CodeInjectionStream::readRel32Jump()
	{
		void *result = peekRel32Jump();
		if (result != NULL)
			skip(Rel32JumpSize);
		return result;
	}

	/// <summary>
	/// Reads a relative 32-bit jump or call instruction from the stream at the current position without advancing the stream position.
	/// </summary>
	/// <returns>The target of the far jump or call if valid, or <c>NULL</c> otherwise.</returns>
	void* CodeInjectionStream::peekRel32Jump() const
	{
		if (!isSpaceAvailable(Rel32JumpSize))
			return NULL;

		// Make sure this is actually a rel32 JMP or CALL instruction
		uint8_t jumpOpcode = *currentPtr;
		if (jumpOpcode != Rel32JumpOpcode && jumpOpcode != Rel32CallOpcode)
			return NULL;

		// target = offset + address of next instruction
		int32_t jumpOffset = *reinterpret_cast<int32_t*>(currentPtr + 1);
		return currentPtr + Rel32JumpSize + jumpOffset;
	}

	/// <summary>
	/// Writes a relative 32-bit jump instruction to the stream at the current position, advancing the stream by the size of the instruction.
	/// </summary>
	/// <param name="target">The target of the far jump instruction to write.</param>
	void CodeInjectionStream::writeRel32Jump(void *target)
	{
		writeRel32Jump(Rel32JumpOpcode, target);
	}

	/// <summary>
	/// Writes a relative 32-bit call instruction to the stream at the current position, advancing the stream by the size of the instruction.
	/// </summary>
	/// <param name="target">The target of the far call instruction to write.</param>
	void CodeInjectionStream::writeRel32Call(void *target)
	{
		writeRel32Jump(Rel32CallOpcode, target);
	}

	/// <summary>
	/// Writes a relative 32-bit jump instruction.
	/// </summary>
	/// <param name="opcode">The opcode.</param>
	/// <param name="target">The target.</param>
	void CodeInjectionStream::writeRel32Jump(uint8_t opcode, void *target)
	{
		if (!isSpaceAvailable(Rel32JumpSize))
			return;

		// offset = target - address of next instruction
		int32_t offset = static_cast<uint8_t*>(target) - (currentPtr + Rel32JumpSize);

		// Write the opcode and the offset
		currentPtr[0] = opcode;
		*reinterpret_cast<int32_t*>(&currentPtr[1]) = offset;
		needsFlush = true;

		// Advance the stream
		currentPtr += Rel32JumpSize;
	}

	/// <summary>
	/// Writes NOP instructions to the stream at the current position, advancing the stream by the size of the data written.
	/// </summary>
	/// <param name="count">The number of NOP instructions to write. Each instruction is one byte large.</param>
	void CodeInjectionStream::writeNops(int count)
	{
		if (!isSpaceAvailable(count))
			return;

		for (int i = 0; i < count; i++)
			currentPtr[i] = NopOpcode;

		currentPtr += count;
	}

	/// <summary>
	/// Seeks to an offset from the beginning of the stream's data block.
	/// </summary>
	/// <param name="offset">The offset to seek to. It must be inside the data block.</param>
	void CodeInjectionStream::seekTo(size_t offset)
	{
		if (offset <= size)
			currentPtr = start + offset;
	}

	/// <summary>
	/// Seeks to a pointer inside the stream's data block.
	/// </summary>
	/// <param name="ptr">The pointer to seek to. It must be inside the data block.</param>
	void CodeInjectionStream::seekTo(const void *ptr)
	{
		seekTo(static_cast<const uint8_t*>(ptr) - start);
	}

	/// <summary>
	/// Skips a number of bytes.
	/// </summary>
	/// <param name="count">The maximum number of bytes to skip.</param>
	void CodeInjectionStream::skip(size_t count)
	{
		if (isSpaceAvailable(count))
			currentPtr += count;
		else
			currentPtr = start + size;
	}

	/// <summary>
	/// Commits any changes made to the code block.
	/// </summary>
	void CodeInjectionStream::flush()
	{
		if (needsFlush)
		{
			Memory::flushCode(start, size);
			needsFlush = false;
		}
	}

	/// <summary>
	/// Unprotects the stream's data block and allows the code inside it to be manipulated.
	/// </summary>
	void CodeInjectionStream::unprotect()
	{
		Memory::unprotectCode(start, size, &oldProtection);
	}

	/// <summary>
	/// Restores the stream's data block's protection to its original setting.
	/// </summary>
	void CodeInjectionStream::protect()
	{
		Memory::protectCode(start, size, oldProtection);
	}
}
