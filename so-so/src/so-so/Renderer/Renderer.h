#pragma once

#include "glm/glm.hpp"

#include "RenderCommand.h"

#include "ShaderLibrary.h"

namespace soso {

	class Mesh;
	class SceneCamera;

	class Renderer {
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static std::shared_ptr<ShaderLibrary> GetShaderLibrary();

		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitMesh(const std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void ImGuiRendererDebug();

	private:
		
	};
}