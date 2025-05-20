#include "sspch.h"
#include "Renderer.h"

#include "Mesh.h"
#include "Material.h"
#include "SceneCamera.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

// Temporary
#include "so-so/RenderAPI/OpenGL/OpenGLRenderer.h"
#include "glad/glad.h"
#include "imgui.h"

namespace soso {

	static RendererAPI* s_RendererAPI = nullptr;
	Statistics Renderer::s_Stats{};

	struct CameraData {
		glm::mat4 ViewProjection;
		glm::mat4 RotationOnlyViewProjection;
		glm::vec3 CameraPos; // make vec4 for std140 alignment
	};

	//static_assert(sizeof(CameraData) % 16 == 0, "Size must be a multiple of 16 bytes");


	struct DirectionalLight {
		glm::vec4 Direction;
		glm::vec4 Ambient;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
	};

	static_assert(sizeof(DirectionalLight) % 16 == 0, "Size must be a multiple of 16 bytes");

	struct RenderData {

		std::shared_ptr<FrameBuffer> CompositeFrameBuffer; // Main render target
		std::shared_ptr<ShaderLibrary> ShaderLibrary;
		std::shared_ptr<Texture2D> WhiteTexture; // Default texture
		std::shared_ptr<UniformBuffer> CameraBufferObject;
		std::shared_ptr<UniformBuffer> LightBufferObject;
		std::shared_ptr<UniformBuffer> TransformBufferObject;
		std::shared_ptr<VertexArray> FSQuadVertexArray;
		
		CameraData CameraData;
		DirectionalLight LightData;


		std::shared_ptr<FrameBuffer> ShadowPassFrameBuffer;

	};

	static RenderData* s_Data = nullptr;

	void Renderer::Init() {
		
		s_RendererAPI = new OpenGLRenderer; 
		s_RendererAPI->Init();

		s_Data = new RenderData;

		// Initialize composite framebuffer
		FrameBufferConfig fbConfig;
		fbConfig.Attachments = { FrameBufferTextureFormat::RGBA8,
								 FrameBufferTextureFormat::RED_INTEGER, 
								 FrameBufferTextureFormat::DEPTH24STENCIL8 };
		
		fbConfig.Width = 1280; // Need to handle resize
		fbConfig.Height = 720;
		s_Data->CompositeFrameBuffer = FrameBuffer::Create(fbConfig);

		// Load default shaders
		s_Data->ShaderLibrary = std::make_shared<ShaderLibrary>();
		s_Data->ShaderLibrary->Load("Resources/Shader/Debug.glsl");
		s_Data->ShaderLibrary->Load("Resources/Shader/Skybox.glsl");
		s_Data->ShaderLibrary->Load("Resources/Shader/BlinnPhong.glsl");
		s_Data->ShaderLibrary->Load("Resources/Shader/FSQuad.glsl");

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

		s_Data->LightData.Direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
		s_Data->LightData.Ambient = glm::vec4(0.9f);
		s_Data->LightData.Diffuse = glm::vec4(1.0f, 0.95f, 0.85f, 1.0f);
		s_Data->LightData.Specular = glm::vec4(1.0f);

		{
			struct QuadVertexData {
				glm::vec3 Pos;
				glm::vec2 TexCoord;
			};

			QuadVertexData* quadData = new QuadVertexData[4];

			quadData[0].Pos = glm::vec3(-1, -1, 0);
			quadData[0].TexCoord = glm::vec2(0, 0);
			quadData[1].Pos = glm::vec3(1, -1, 0);
			quadData[1].TexCoord = glm::vec2(1, 0);
			quadData[2].Pos = glm::vec3(1, 1, 0);
			quadData[2].TexCoord = glm::vec2(1, 1);
			quadData[3].Pos = glm::vec3(-1, 1, 0);
			quadData[3].TexCoord = glm::vec2(0, 1);

			s_Data->FSQuadVertexArray = VertexArray::Create();

			VertexBufferLayout vbLayout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			};
			
			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

			auto vb = VertexBuffer::Create(quadData, 4 * sizeof(QuadVertexData));
			vb->SetLayout(vbLayout);

			s_Data->FSQuadVertexArray->AddVertexBuffer(vb);
			s_Data->FSQuadVertexArray->SetIndexBuffer(IndexBuffer::Create(indices, 6 * sizeof(uint32_t)));

			delete[] quadData;
		}
	}

	void Renderer::Shutdown() {

		delete s_Data;
		delete s_RendererAPI;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {

		s_RendererAPI->SetViewport(0, 0, width, height);
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
	
	void Renderer::Clear(float r, float g, float b, float a) {
		
		s_RendererAPI->Clear();
	}

	void Renderer::BeginScene(SceneCamera& camera) {

		s_Stats.DrawCalls = 0;
		s_Stats.Meshes = 0;

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

		s_RendererAPI->DrawIndexed(vertexArray);

		s_Stats.DrawCalls++;
	}

	void Renderer::SubmitFullscreenQuad() {

		auto&& shader = s_Data->ShaderLibrary->Get("FSQuad");
		shader->Bind();

		s_RendererAPI->DrawIndexed(s_Data->FSQuadVertexArray, 6);
	}

	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, const std::shared_ptr<Material>& materialOverride) {

		mesh->m_VertexArray->Bind();

		const auto& materials = mesh->m_Materials;
		

		// Tracks last bound material to skip redundant Bind() calls
		const Material* lastUsedMat = nullptr;

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (auto&& submesh : mesh->m_Submeshes) {

			const auto& material = (materialOverride) ? materialOverride : materials[submesh.MaterialIndex];

			if (material.get() != lastUsedMat) { material->Bind(); lastUsedMat = material.get(); }

			glm::mat4 localTransform = transform * submesh.Transform;

			s_Data->TransformBufferObject->SetData(&localTransform, sizeof(localTransform));
			
			glDrawElementsBaseVertex(GL_TRIANGLES,
				submesh.IndexCount,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * submesh.BaseIndex), 
				submesh.BaseVertex);

			s_Stats.DrawCalls++;
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		s_Stats.Meshes++;
	}


	void Renderer::SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray) {

		s_RendererAPI->SetDepthFunction(RendererAPI::DepthFunction::LEQUAL);
		vertexArray->Bind();
		shader->Bind();
		
		s_RendererAPI->DrawIndexed(vertexArray);
		vertexArray->Unbind();
		s_RendererAPI->SetDepthFunction(RendererAPI::DepthFunction::LEQUAL);

		s_Stats.DrawCalls++;
	}


	//=============================================================================================================================


	void Renderer::SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color) {


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