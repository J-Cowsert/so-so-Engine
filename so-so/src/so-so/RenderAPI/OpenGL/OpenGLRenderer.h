#pragma once

#include "so-so/Renderer/RendererAPI.h"

namespace soso {

	class OpenGLRenderer : public RendererAPI {

		void Init();

		void SetDepthFunction(DepthFunction func) override;
		void EnableDepthTest() override;
		
		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetLineWidth(float width);

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override final;
		void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override final;


		// TODO:

		void DrawMesh();
		void DrawQuad();
	};
}