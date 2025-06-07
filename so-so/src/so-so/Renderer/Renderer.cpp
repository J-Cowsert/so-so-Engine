#include "sspch.h"
#include "Renderer.h"

#include "Mesh.h"
#include "Material.h"
#include "SceneCamera.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

#include "so-so/ImGui/ImGuiWidgets.h"
// Temporary
#include "so-so/RenderAPI/OpenGL/OpenGLRenderer.h"
#include "glad/glad.h"
#include "imgui.h"

namespace soso {

	static RendererAPI* s_RendererAPI = nullptr;
	Statistics Renderer::s_Stats{};

	void Renderer::Init() {
		
		s_RendererAPI = new OpenGLRenderer; 
		s_RendererAPI->Init();

		s_Data = new SceneRendererData;
		auto& data = s_Data;

		FrameBufferConfig compositeConfig;
		compositeConfig.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::DEPTH24STENCIL8 };
		compositeConfig.Width = 1280; // Need to handle resizing, also not hardcode this.
		compositeConfig.Height = 720;
		data->CompositeFrameBuffer = FrameBuffer::Create(compositeConfig);

		FrameBufferConfig shadowConfig;
		shadowConfig.Attachments = { FrameBufferTextureFormat::DEPTH32F };
		shadowConfig.Width = 4096;
		shadowConfig.Height = 4096;
		data->ShadowPassFrameBuffer = FrameBuffer::Create(shadowConfig);

		// Load shaders
		data->ShaderLibrary = std::make_shared<ShaderLibrary>();
		data->ShaderLibrary->Load("Resources/Shader/PBR.glsl");
		data->ShaderLibrary->Load("Resources/Shader/Debug.glsl");
		data->ShaderLibrary->Load("Resources/Shader/Skybox.glsl");
		data->ShaderLibrary->Load("Resources/Shader/FSQuad.glsl");
		data->ShaderLibrary->Load("Resources/Shader/DirectionalShadowMap.glsl");
		
		data->ShadowMapShader = data->ShaderLibrary->Get("DirectionalShadowMap");

		// Create default white texture
		TextureConfig config;
		config.Format = ImageFormat::RGBA8;
		config.Width = 1;
		config.Height = 1;
		uint32_t whiteTextureData = 0xffffffff;
		data->WhiteTexture = Texture2D::Create(config, ByteBuffer(&whiteTextureData, sizeof(uint32_t)));

		// Create global UniformBuffers
		data->CameraBufferUBObject = UniformBuffer::Create(sizeof(CameraUBData), 0);
		data->TransformBufferUBObject = UniformBuffer::Create(sizeof(glm::mat4), 1);
		data->LightBufferUBObject = UniformBuffer::Create(sizeof(DirectionalLightUBData), 2);
		data->ShadowLightViewUBObject = UniformBuffer::Create(sizeof(DirectionalShadowMapUBData), 4);

		// Default Directional light settings
		data->FrameData.DirLightUBData.Direction = glm::vec4(0.0f, -1.0f, 0.03f, 1.0f);
		data->FrameData.DirLightUBData.Ambient = glm::vec4(0.9f);
		data->FrameData.DirLightUBData.Diffuse = glm::vec4(1.0f);
		data->FrameData.DirLightUBData.Specular = glm::vec4(1.0f);

		{
			float cubeVertices[] = {
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f 
			};

			uint32_t cubeIndices[] = {
				
				0, 1, 2, // Back face
				2, 3, 0,
				
				4, 5, 1, // Left face
				1, 0, 4,
				
				7, 6, 5, // Front face
				5, 4, 7,
				
				3, 2, 6, // Right face
				6, 7, 3,
				
				4, 0, 3, // Top face
				3, 7, 4,
				
				1, 5, 6, // Bottom face
				6, 2, 1
			};

			data->CubeVertexArray = VertexArray::Create();

			VertexBufferLayout vbLayout = { 
				{ soso::ShaderDataType::Float3, "a_Position" } 
			};

			auto vb = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
			vb->SetLayout(vbLayout);
			data->CubeVertexArray->AddVertexBuffer(vb);
			data->CubeVertexArray->SetIndexBuffer(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices)));
		}

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

			data->FSQuadVertexArray = VertexArray::Create();

			VertexBufferLayout vbLayout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			};
			
			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

			auto vb = VertexBuffer::Create(quadData, 4 * sizeof(QuadVertexData));
			vb->SetLayout(vbLayout);

			data->FSQuadVertexArray->AddVertexBuffer(vb);
			data->FSQuadVertexArray->SetIndexBuffer(IndexBuffer::Create(indices, 6 * sizeof(uint32_t)));

			delete[] quadData;
		}


		// Skybox Material
		data->SkyboxMaterial = Material::Create(data->ShaderLibrary->Get("Skybox"), "skybox");

	}

	void Renderer::Shutdown() {

		delete s_Data;
		delete s_RendererAPI;
	}

	void Renderer::BeginScene(SceneCamera& camera) {

		
		if (s_Resize) {

			s_Data->ShadowPassFrameBuffer->Resize((uint32_t)s_WindowSize.x, (uint32_t)s_WindowSize.y);
			s_Data->CompositeFrameBuffer->Resize((uint32_t)s_WindowSize.x, (uint32_t)s_WindowSize.y);
			s_Resize = false;
		}

		s_Stats.DrawCalls = 0;
		s_Stats.Meshes = 0;

		auto& data = s_Data;
		FrameData& frameData = s_Data->FrameData;

		auto& cameraData = frameData.CameraUBData;
		cameraData.ViewProjection = camera.GetViewProjection();
		cameraData.RotationOnlyViewProjection = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		cameraData.CameraPos = glm::vec4(camera.GetPosition(), 1.0f);

		auto& lightData = frameData.DirLightUBData;
		auto& dirShadowData = frameData.DirShadowMapUBData;

		glm::vec3 lightDir = glm::normalize(glm::vec3(lightData.Direction));
		glm::vec3 eye = -lightDir * 20.0f; // push back so ortho covers scene
		glm::vec3 target = glm::vec3(0.0f);
		auto nearPlane = 0.1f, farPlane = 50.0f;

		auto&& LightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearPlane, farPlane);
		auto&& LightView = glm::lookAt(eye, target + lightDir, glm::vec3(0, 1, 0));
		dirShadowData.LightViewProjection = LightProjection * LightView;

		data->CameraBufferUBObject->SetData(&cameraData, sizeof(cameraData));
		data->LightBufferUBObject->SetData(&lightData, sizeof(DirectionalLightUBData));
		data->ShadowLightViewUBObject->SetData(&dirShadowData, sizeof(DirectionalShadowMapUBData));
	}

	void Renderer::EndScene() {

		FlushDrawList();
	}

	void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		shader->Bind();
		s_Data->TransformBufferUBObject->SetData(&transform, sizeof(transform));
		s_RendererAPI->DrawIndexed(vertexArray);
		s_Stats.DrawCalls++;
	}

	void Renderer::RenderFullscreenQuad() {

		auto&& shader = s_Data->ShaderLibrary->Get("FSQuad");
		shader->Bind();
		s_RendererAPI->DrawIndexed(s_Data->FSQuadVertexArray, 6);
		s_Stats.DrawCalls++;
	}


	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, const std::shared_ptr<Material>& materialOverride) {

		// TODO: batching, culling, etc

		DrawCommand& command = s_Data->DrawList.emplace_back();
		command.Mesh = mesh;
		command.Transform = transform;
		command.MaterialOverride = materialOverride;

		s_Stats.DrawCalls += (uint32_t)mesh->m_Submeshes.size();
		s_Stats.Meshes++;
	}

	void Renderer::SubmitQuad(std::shared_ptr<Material> material, const glm::mat4& transform) {

		QuadCommand& command = s_Data->QDrawList.emplace_back();
		command.Material = material;
		command.Transform = transform;
		s_Stats.DrawCalls++;
	}

	void Renderer::SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color) {

		SS_CORE_ASSERT(false, "Not implemented");
	}

	void Renderer::ShadowPass() {

		auto& data = s_Data;

		data->ShadowPassFrameBuffer->Bind();
		Clear(1, 0, 1, 1);

		auto& shadowMapShader = data->ShadowMapShader;
		auto& transformUB = data->TransformBufferUBObject;

		for (auto&& dc : data->DrawList) {

			s_RendererAPI->DrawMesh(dc.Mesh, transformUB, dc.Transform, dc.MaterialOverride, shadowMapShader);
		}

		data->ShadowPassFrameBuffer->Unbind();
	}


	void Renderer::FlushDrawList() {

		ShadowPass();
		
		auto& data = s_Data;
		data->CompositeFrameBuffer->Bind();
		
		auto& transformUB = data->TransformBufferUBObject;
		
		{
			Clear(0.2f, 0.2f, 0.2f, 1.0f);

			// skybox pass

			if (data->SkyboxTexture) {
			
				//s_RendererAPI->DrawSkybox(data->SkyboxMaterial, data->CubeVertexArray);
				//s_Stats.DrawCalls++;
			}


			// opaque geometry pass

			glBindTextureUnit(7, data->ShadowPassFrameBuffer->GetDepthAttachmentRendererID());
			for (auto&& dc : data->DrawList) {

				s_RendererAPI->DrawMesh(dc.Mesh, transformUB, dc.Transform, dc.MaterialOverride);
			}

			// quad pass

			for (const auto& dc : data->QDrawList) {

				dc.Material->GetShader()->Bind();
				dc.Material->Bind();
				transformUB->SetData(&dc.Transform, sizeof(dc.Transform));
				s_RendererAPI->DrawIndexed(data->FSQuadVertexArray);
			}

		}
		data->CompositeFrameBuffer->Unbind();

		data->DrawList.clear();
		data->QDrawList.clear();
	}

	//=============================================================================================================================


	void Renderer::SetSkyboxTexture(std::shared_ptr<TextureCube> texture) {

		//s_Data->SkyboxTexture = texture;

		s_Data->SkyboxMaterial->Set("u_Skybox", texture);
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

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {

		s_RendererAPI->SetViewport(0, 0, width, height);

		s_WindowSize = { width, height };
		s_Resize = true;
	}

	void Renderer::Clear(float r, float g, float b, float a) {
		s_RendererAPI->SetClearColor(r, g, b, a);
		s_RendererAPI->Clear();
	}

	void Renderer::ImGuiRendererDebug() {

		auto& frameData = s_Data->FrameData;

		static bool showShadowMap = false;

		ImGui::Begin("Renderer");
		{
			ImGui::BeginChild("Lighting");
			{

				ImGui::SliderFloat3("Direction", &frameData.DirLightUBData.Direction[0], -1.0f, 1.0f);
				static float ambiant = 1.0f;
				ImGui::SliderFloat("Ambient", &ambiant, 0.0f, 5.0f);
				frameData.DirLightUBData.Ambient = glm::vec4(glm::vec3(ambiant), 1.0);
				ImGui::SliderFloat3("Diffuse", &frameData.DirLightUBData.Diffuse[0], 0.0f, 1.0f);
				ImGui::SliderFloat3("Specular", &frameData.DirLightUBData.Specular[0], 0.0f, 1.0f);

				ImGui::Separator();
				UI::ToggleSwitch("View Shadow Map", &showShadowMap);
			}
			ImGui::EndChild();

		}
		ImGui::End();
		
		if (showShadowMap) {

			ImGui::Begin("ShadowMap");
			{
				ImVec2 panelSize = ImGui::GetContentRegionAvail();
				uint32_t depthTexID = s_Data->ShadowPassFrameBuffer->GetDepthAttachmentRendererID();
				ImGui::Image((ImTextureID)(intptr_t)depthTexID, panelSize, ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::End();
		}
	}
}