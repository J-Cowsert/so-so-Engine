#include "sspch.h"
#include "Renderer.h"

namespace soso {

	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init() {
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(SceneCamera& camera) {

		s_SceneData->ViewProjection = camera.GetViewProjection();

		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		s_SceneData->SkyboxViewProjection = camera.GetProjectionMatrix() * skyboxView;
	}

	void Renderer::EndScene() {

	}

	void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjection);
		shader->SetMat4("u_Model", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray) {

		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->SkyboxViewProjection);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}