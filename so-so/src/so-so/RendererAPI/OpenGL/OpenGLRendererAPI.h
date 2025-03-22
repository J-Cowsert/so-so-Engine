#pragma once

#include "so-so/Renderer/RendererAPI.h"

namespace soso {

	class OpenGLRendererAPI : public RendererAPI {
		void Init();

		void SetDepthFunction(DepthFunction func) override;
		void EnableDepthTest() override;
		void EnableCulling() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;
		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
		void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray) override;
		void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount);
	};

}