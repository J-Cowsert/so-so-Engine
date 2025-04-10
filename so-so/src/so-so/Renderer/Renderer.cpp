#include "sspch.h"
#include "Renderer.h"

// Temporary
#include "glad/glad.h"
#include "imgui.h"

namespace soso {

	struct DirectionalLight {
		glm::vec3 Direction;
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;
	};

	struct RenderData {

		std::shared_ptr<ShaderLibrary> ShaderLibrary;

		glm::mat4 ViewProjection;
		glm::mat4 SkyboxViewProjection;

		DirectionalLight DirLight;
	};

	static RenderData* s_Data = nullptr;

	void Renderer::Init() {
		
		s_Data = new RenderData;
		s_Data->ShaderLibrary = std::make_shared<ShaderLibrary>();

		Renderer::GetShaderLibrary()->Load("Resources/Shader/Phong.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shader/Skybox.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shader/Texture.glsl");

		// Temporary
		s_Data->DirLight.Direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		s_Data->DirLight.Ambient = glm::vec3(0.9f, 0.9f, 0.9f);
		s_Data->DirLight.Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		s_Data->DirLight.Specular = glm::vec3(1.0f, 1.0f, 1.0f);

		RenderCommand::Init();
	}

	void Renderer::Shutdown() {

		delete s_Data;

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {

		RenderCommand::SetViewport(0, 0, width, height);
	}

	std::shared_ptr<ShaderLibrary> Renderer::GetShaderLibrary() {
		return s_Data->ShaderLibrary;
	}
	
	void Renderer::BeginScene(SceneCamera& camera) {

		s_Data->ViewProjection = camera.GetViewProjection();

		// Remove position components from the view matrix. We only want the skybox to move when we rotate, and not when we change positions. 
		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));

		s_Data->SkyboxViewProjection = camera.GetProjection() * skyboxView;
	}

	void Renderer::EndScene() {}

	void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
		shader->SetMat4("u_Model", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitMesh(const std::shared_ptr<Mesh> mesh, const glm::mat4& transform) {

		mesh->m_VertexArray->Bind();
		mesh->m_VertexBuffer->Bind();
		mesh->m_IndexBuffer->Bind();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto& materials = mesh->m_Materials;

		
		for (Submesh& submesh : mesh->GetSubmeshes()) {

			auto& mat = materials[submesh.MaterialIndex];
			auto& shader = mat->GetShader();

			mat->Bind();
			shader->Bind();
			
			shader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
			shader->SetMat4("u_Transform", transform * submesh.Transform);

			shader->SetFloat3("u_DirLight.Direction", s_Data->DirLight.Direction);
			shader->SetFloat3("u_DirLight.Ambient", s_Data->DirLight.Ambient);
			shader->SetFloat3("u_DirLight.Diffuse", s_Data->DirLight.Diffuse);
			shader->SetFloat3("u_DirLight.Specular", s_Data->DirLight.Specular);
			

			glDrawElementsBaseVertex(GL_TRIANGLES,
				submesh.IndexCount,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * submesh.BaseIndex), 
				submesh.BaseVertex);
		}
	
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray) {

		RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LEQUAL);
		vertexArray->Bind();
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_Data->SkyboxViewProjection);
		
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
		RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LESS);
	}

	void Renderer::ImGuiRendererDebug() {

		ImGui::Begin("Directional Light Settings");

		ImGui::Text("Direction");
		ImGui::SliderFloat3("Direction", &s_Data->DirLight.Direction[0], -1.0f, 1.0f);
		//s_Data->DirLight.Direction = glm::normalize(s_Data->DirLight.Direction);

		ImGui::Text("Ambient");
		ImGui::SliderFloat3("Ambient", &s_Data->DirLight.Ambient[0], 0.0f, 1.0f);
		ImGui::Text("Diffuse");
		ImGui::SliderFloat3("Diffuse", &s_Data->DirLight.Diffuse[0], 0.0f, 1.0f);
		ImGui::Text("Specular");
		ImGui::SliderFloat3("Specular", &s_Data->DirLight.Specular[0], 0.0f, 1.0f);

		ImGui::End();
	}
}