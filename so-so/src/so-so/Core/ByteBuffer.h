#pragma once

#include "Core.h"

#include <stdint.h>
#include <cstring>

namespace soso {

	// Non-owning raw buffer class : You are responsible for releasing memory

	struct ByteBuffer {

		void* Data = nullptr;
		uint64_t Size = 0;

		ByteBuffer() = default;

		ByteBuffer(const void* data, uint64_t size = 0)
			: Data((void*)data), Size(size)
		{
		}

		static ByteBuffer Copy(const void* data, uint64_t size) {

			ByteBuffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(uint64_t size) {
			
			Release();
			if (size == 0) return;
			Data = (void*)malloc(size);
			Size = size;
		}

		void Release() {

			free(Data);
			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize() {

			if (Data)
				memset(Data, 0, Size);
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0) {

			SS_CORE_ASSERT(size + offset <= Size, "Buffer Overflow");
			memcpy(reinterpret_cast<uint8_t*>(Data) + offset, data, size);
		}

		template<typename T>
		T* As() const {
			return (T*)Data;
		}

		operator bool() const {
			return (bool)Data;
		}

	};
}