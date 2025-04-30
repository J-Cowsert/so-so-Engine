#include "sspch.h"
#include "Renderer.h"

#include "Mesh.h"
#include "Material.h"
#include "SceneCamera.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

// Temporary
#include "glad/glad.h"
#include "imgui.h"

namespace soso {

	struct CameraData {
		glm::mat4 ViewProjection;
		glm::mat4 RotationOnlyViewProjection;
		glm::vec3 CameraPos;
	};

	struct DirectionalLight {
		glm::vec4 Direction;
		glm::vec4 Ambient;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
	};

	struct RenderData {

		std::shared_ptr<FrameBuffer> CompositeFrameBuffer; // Main render target

		std::shared_ptr<ShaderLibrary> ShaderLibrary;

		std::shared_ptr<Texture2D> WhiteTexture; // Default texture

		std::shared_ptr<UniformBuffer> CameraBufferObject;
		std::shared_ptr<UniformBuffer> LightBufferObject;
		std::shared_ptr<UniformBuffer> TransformBufferObject;

		CameraData CameraData;
		DirectionalLight LightData;
	};

	static RenderData* s_Data = nullptr;

	void Renderer::Init() {
		
		s_Data = new RenderData;

		// Initialize Compositing framebuffer
		FrameBufferConfig fbConfig;
		fbConfig.Attachments = { FrameBufferTextureFormat::RGBA8,
								 FrameBufferTextureFormat::RED_INTEGER, 
								 FrameBufferTextureFormat::DEPTH24STENCIL8 };
		fbConfig.Width = 1280;
		fbConfig.Height = 720;
		s_Data->CompositeFrameBuffer = FrameBuffer::Create(fbConfig);

		// Load default shaders
		s_Data->ShaderLibrary = std::make_shared<ShaderLibrary>();
		Renderer::GetShaderLibrary()->Load("Debug", "Resources/Shader/Debug.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shader/Skybox.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shader/BlinnPhong.glsl");

		// Create default white texture
		TextureConfig config;
		config.Format = ImageFormat::RGBA8;
		config.Width = 1;
		config.Height = 1;
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture = Texture2D::Create(config, ByteBuffer(&whiteTextureData, sizeof(uint32_t)));

		// Create global UniformBuffers
		s_Data->CameraBufferObject = UniformBuffer::Create(sizeof(CameraData), 0);
		s_Data->TransformBufferObject = UniformBuffer::Create(sizeof(glm::mat4), 1);
		s_Data->LightBufferObject = UniformBuffer::Create(sizeof(DirectionalLight), 2);


		s_Data->LightData.Direction = glm::vec4(-0.0f, -1.0f, -0.0f, 0.1f);
		s_Data->LightData.Ambient = glm::vec4(0.5f);
		s_Data->LightData.Diffuse = glm::vec4(1.0f, 0.95f, 0.85f, 1.0f);
		s_Data->LightData.Specular = glm::vec4(1.0f);

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

	std::shared_ptr<FrameBuffer> Renderer::GetCompositeFrameBuffer() {	

		return s_Data->CompositeFrameBuffer;
	}

	std::shared_ptr<Texture2D> Renderer::GetWhiteTexture() {

		return s_Data->WhiteTexture;
	}
	
	void Renderer::BeginScene(SceneCamera& camera) {

		auto& cameraData = s_Data->CameraData;

		cameraData.ViewProjection = camera.GetViewProjection();
		cameraData.RotationOnlyViewProjection = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		cameraData.CameraPos = camera.GetPosition();

		auto& lightData = s_Data->LightData;

		s_Data->CameraBufferObject->SetData(&cameraData, sizeof(CameraData));
		s_Data->LightBufferObject->SetData(&lightData, sizeof(DirectionalLight));
	}

	void Renderer::EndScene() {}

	void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		shader->Bind();

		s_Data->TransformBufferObject->SetData(&transform, sizeof(transform));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, const std::shared_ptr<Material>& materialOverride) {

		auto& cameraData = s_Data->CameraData;

		mesh->m_VertexArray->Bind();
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		const auto& materials = mesh->m_Materials;
		
		for (Submesh& submesh : mesh->GetSubmeshes()) {

			const auto& material = (materialOverride) ? materialOverride : materials[submesh.MaterialIndex];

			material->Bind();

			glm::mat4 localTransform = transform * submesh.Transform;
			s_Data->TransformBufferObject->SetData(&localTransform, sizeof(localTransform));
			
			// TODO: Move to RenderAPI
			glDrawElementsBaseVertex(GL_TRIANGLES,
				submesh.IndexCount,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * submesh.BaseIndex), 
				submesh.BaseVertex);
		}
	
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray) {

		RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LEQUAL);
		vertexArray->Bind();
		shader->Bind();
		
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
		RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LESS);
	}

	void Renderer::ImGuiRendererDebug() {

		ImGui::Begin("Directional Light Settings");

		ImGui::Text("Direction");
		ImGui::SliderFloat3("Direction", &s_Data->LightData.Direction[0], -1.0f, 1.0f);
		ImGui::Text("Ambient");
		ImGui::SliderFloat3("Ambient", &s_Data->LightData.Ambient[0], 0.0f, 1.0f);
		ImGui::Text("Diffuse");
		ImGui::SliderFloat3("Diffuse", &s_Data->LightData.Diffuse[0], 0.0f, 1.0f);
		ImGui::Text("Specular");
		ImGui::SliderFloat3("Specular", &s_Data->LightData.Specular[0], 0.0f, 1.0f);

		ImGui::End();
	}
}