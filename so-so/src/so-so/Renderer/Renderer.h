#pragma once

#include "RenderCommand.h"
#include "SceneCamera.h"
#include "Shader.h"


namespace soso {

	class Renderer {
	public:
		static void Init();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData {
			glm::mat4 ViewProjection;
			glm::mat4 SkyboxViewProjection;
		};

		static std::unique_ptr<SceneData> s_SceneData;
	};
}