#pragma once

#include "Mesh.h"
#include "UniformBuffer.h"
#include "VertexArray.h"

#include "glm/glm.hpp"

namespace soso {

	enum class PrimitiveType {
		None = 0,
		Triangles,
		Lines
	};

	class RendererAPI {
	public:
		enum class API {
			None = 0, OpenGL = 1
		};

	public: 
		inline static API GetAPI() { return s_API; }

		virtual void Init() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetLineWidth(float width) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void Clear() = 0;

		virtual void DispatchCompute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0, PrimitiveType type = PrimitiveType::Triangles) = 0;
		virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

		virtual void DrawSkybox(const std::shared_ptr<Material> material, const std::shared_ptr<VertexArray>& cubeVertexArray) = 0; // TODO: Move cube vertex array into opengl renderer.

		virtual void DrawMesh(Mesh* mesh,
							UniformBuffer* transformUB,
							const glm::mat4& transform = glm::mat4(1.0f), 
							Material* materialOverride = nullptr, 
							const Shader* shaderOverride = nullptr) = 0;

		//virtual void DrawSubmesh(const Mesh* mesh, uint32_t submeshIndex, UniformBuffer* transformUB, const glm::mat4& transform = glm::mat4(1.0f), const Material* materialOverride, const Shader* shaderOverride) = 0;
		virtual void DrawQuad(std::shared_ptr<Material> material, const glm::mat4& transform = glm::mat4(1.0f)) = 0;

	private:

		inline static API s_API = RendererAPI::API::OpenGL;
	};
}