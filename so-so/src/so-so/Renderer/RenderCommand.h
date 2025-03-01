#pragma once

#include "RendererAPI.h"

namespace soso {

	class RenderCommand {
	public:
		using DepthFunction = RendererAPI::DepthFunction;

	public: 
		static void Init() {
			s_RendererAPI->Init();
		}

		static void SetDepthFunction(DepthFunction func) {
			s_RendererAPI->SetDepthFunction(func);
		}

		static void EnableDepthTest() {
			s_RendererAPI->EnableDepthTest();
		}

		static void EnableCulling() {
			s_RendererAPI->EnableCulling();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear() {
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		static void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray) {
			s_RendererAPI->DrawArrays(vertexArray);
		}

		static void DrawLines() {

		}
	private:

		static RendererAPI* s_RendererAPI;
	};
}
