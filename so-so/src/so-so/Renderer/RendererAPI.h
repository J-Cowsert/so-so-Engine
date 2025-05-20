#pragma once

#include "glm/glm.hpp"
#include "VertexArray.h"

namespace soso {

	class RendererAPI {
	public:
		enum class API {
			None = 0, OpenGL = 1
		};

		
		enum class DepthFunction {
			LESS,
			LEQUAL,
			GREATER,
			GEQUAL,
			EQUAL,
			ALWAYS,
			NEVER
		};

	public: 
		virtual void Init() = 0;

		virtual void SetDepthFunction(DepthFunction func) = 0;
		virtual void EnableDepthTest() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

		inline static API GetAPI() { return s_API; }
	private:

		inline static API s_API = RendererAPI::API::OpenGL;
	};
}