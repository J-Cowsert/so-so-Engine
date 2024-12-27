#pragma once

namespace soso {

	enum class ShaderDataType {
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t GetShaderDataTypeSize(ShaderDataType type) {
		
		switch (type) 
		{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}

		SS_CORE_ASSERT(false, "Failed: Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement {

		ShaderDataType Type;
		std::string Name;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		// Returns the number of elements that comprise a single instance of the specified data type.
		uint32_t GetComponentSize() const {

			switch (Type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:		return 3; // 3 * Float3
				case ShaderDataType::Mat4:		return 4; // 3 * Float4
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}

			SS_CORE_ASSERT(false, "Failed: Unknown ShaderDataType");
			return 0;
		}

	};

	class BufferLayout {
	public:
		BufferLayout() = default;
		virtual ~BufferLayout() {}

		// Utilizing std::initializer_list in the constructor simplifies the process
		// of instantiating BufferLayout instances by allowing initialization with a
		// list of BufferElement objects directly.
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }

		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:

		// Calculates the memory offsets for each buffer element and computes
		// the total stride of the buffer layout.
		void CalculateOffsetsAndStride() {
			
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};



	/// <summary>
	/// Abstract base class for vertex buffers, providing an interface for managing
	/// vertex data used in graphics rendering.
	/// </summary>
	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		/// <summary>
		/// Creates a concrete instance of VertexBuffer with the provided vertex data.
		/// </summary>
		/// <param name="vertices">A pointer to the array of vertex data.</param>
		/// <param name="size">The size of the vertex data array in bytes.</param>
		/// <returns>A pointer to the created VertexBuffer instance.</returns>
		static VertexBuffer* Create(float* verticies, uint32_t size);
	};

	/// <summary>
	/// Abstract base class for index buffers, providing an interface for managing
	/// index data used in graphics rendering.
	/// </summary>
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		/// <summary>
		/// Creates a concrete instance of IndexBuffer with the provided index data.
		/// </summary>
		/// <param name="indices">A pointer to the array of index data.</param>
		/// <param name="count">The number of indices in the array.</param>
		/// <returns>A pointer to the created IndexBuffer instance.</returns>
		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};
}