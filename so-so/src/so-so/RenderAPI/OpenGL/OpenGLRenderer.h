#pragma once

#include "so-so/Renderer/RendererAPI.h"

namespace soso {

	class OpenGLRenderer : public RendererAPI {

		void Init();

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetLineWidth(float width);

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0, PrimitiveType type = PrimitiveType::Triangles) override final;
		void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override final;

		void DrawSkybox(const std::shared_ptr<Material> material, const std::shared_ptr<VertexArray>& vertexArray) override final;
		void DrawMesh(std::shared_ptr<Mesh>& mesh, std::shared_ptr<UniformBuffer>& transformUB, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr, const std::shared_ptr<Shader>& shaderOverride = nullptr) override final;
		
		void DrawQuad(std::shared_ptr<Material> material, const glm::mat4& transform = glm::mat4(1.0f)) override final {}
	};
}