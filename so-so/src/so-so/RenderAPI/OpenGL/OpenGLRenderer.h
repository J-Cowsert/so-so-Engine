#pragma once

#include "so-so/Renderer/RendererAPI.h"

namespace soso {

	class OpenGLRenderer : public RendererAPI {

		void Init();

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetLineWidth(float width) override final;
		void SetClearColor(float r, float g, float b, float a) override;
		void Clear() override;

		void DispatchCompute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override final;

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0, PrimitiveType type = PrimitiveType::Triangles) override final;
		void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override final;

		void DrawSkybox(const std::shared_ptr<Material> material, const std::shared_ptr<VertexArray>& vertexArray) override final;

		void DrawMesh(Mesh* mesh,  
						UniformBuffer* transformUB,
						const glm::mat4& transform = glm::mat4(1.0f), 
						Material* materialOverride = nullptr, 
						const Shader* shaderOverride = nullptr) override final;

		//void DrawSubmesh(const Mesh* mesh, 
		//					uint32_t submeshIndex, 
		//					UniformBuffer* transformUB, 
		//					const glm::mat4& transform = glm::mat4(1.0f), 
		//					const Material* materialOverride, 
		//					const Shader* shaderOverride);

		void DrawQuad(std::shared_ptr<Material> material, const glm::mat4& transform = glm::mat4(1.0f)) override final {} // temp
	};
}