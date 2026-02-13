#include "sspch.h"
#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Profiler.h"

#include "ShaderLibrary.h"
#include "FrameBuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneCamera.h"

#include "so-so/ImGui/ImGuiWidgets.h"

#include "so-so/RenderAPI/OpenGL/OpenGLRenderer.h"
#include "imgui.h"

// Temporary
#include "glad/glad.h"

namespace soso {

	static RendererAPI* s_RendererAPI = nullptr;
	
	struct RendererSettings {
		
		bool SkyboxPass = true;

		bool ShadowPass = false;
		bool WirePass = false;

		float ShadowFrustumBounds = 50.0f;
		float ShadowNearPlane = 0.001f, ShadowFarPlane = 50.0f;
		float ShadowEyeFactor = 20.0f;
	};
	
	inline static RendererSettings s_RendererSettings;

	void Renderer::Init() {

		SS_PROFILE_FUNCTION();
		
		s_RendererAPI = new OpenGLRenderer; 
		s_RendererAPI->Init();

		s_Data = new RendererData;

		auto& data = *s_Data;
		auto& frameData = data.FrameData;

		FrameBufferConfig geometryConfig;
		geometryConfig.Attachments = { ImageFormat::RGBA32F, ImageFormat::DEPTH24STENCIL8 };
		geometryConfig.Width = 1920;
		geometryConfig.Height = 1080;
		data.GeometryFrameBuffer = FrameBuffer::Create(geometryConfig);

		FrameBufferConfig shadowConfig;
		shadowConfig.Attachments = { ImageFormat::DEPTH32F };
		shadowConfig.Width = 2048;
		shadowConfig.Height = 2048;
		data.ShadowPassFrameBuffer = FrameBuffer::Create(shadowConfig);

		FrameBufferConfig compositeConfig;
		compositeConfig.Attachments = { ImageFormat::RGBA8 };
		compositeConfig.Width = 1920; // Need to handle resizing, also not hardcode this.
		compositeConfig.Height = 1080;
		compositeConfig.Samples = 1;
		data.CompositeFrameBuffer = FrameBuffer::Create(compositeConfig);

		// Load shaders
		data.ShaderLib = std::make_shared<ShaderLibrary>();
		data.ShaderLib->Load("Resources/Shader/PBR.glsl");
		data.ShaderLib->Load("Resources/Shader/Debug.glsl");
		data.ShaderLib->Load("Resources/Shader/MeshWireframe.glsl");
		data.ShaderLib->Load("Resources/Shader/Skybox.glsl");
		data.ShaderLib->Load("Resources/Shader/FSQuad.glsl");
		data.ShaderLib->Load("Resources/Shader/DirectionalShadowMap.glsl");
		data.ShaderLib->Load("Resources/Shader/Line.glsl");
		data.ShaderLib->Load("Resources/Shader/PostProcess.glsl");

		data.ShaderLib->Load("Resources/Shader/EquirectangularToCubeCompute.glsl");
		data.ShaderLib->Load("Resources/Shader/IrradianceMapCompute.glsl");
		data.ShaderLib->Load("Resources/Shader/RadianceMapCompute.glsl");

		data.ShadowMapShader = data.ShaderLib->Get("DirectionalShadowMap");
		data.FinalPassShader = data.ShaderLib->Get("PostProcess");

		data.FinalPassMaterial = Material::Create(data.FinalPassShader, "Uniform", "FinalPass");
		data.SceneData.EnviromentMaterial = Material::Create(data.ShaderLib->Get("PBR"));
		data.SceneData.SkyboxMaterial = Material::Create(data.ShaderLib->Get("Skybox"));


		// Create default white texture
		TextureConfig config;
		config.Format = ImageFormat::RGBA8;
		config.Width = 1;
		config.Height = 1;
		uint32_t whiteTextureData = 0xffffffff;
		data.WhiteTexture = Texture2D::Create(config, ByteBuffer(&whiteTextureData, sizeof(uint32_t)));


		// Create global UniformBuffers
		frameData.CameraBufferUBObject = UniformBuffer::Create(sizeof(CameraUBData), 0);
		frameData.LightBufferUBObject = UniformBuffer::Create(sizeof(DirectionalLightUBData), 2);
		frameData.ShadowLightViewUBObject = UniformBuffer::Create(sizeof(DirectionalShadowMapUBData), 4);
		frameData.TransformUBObject = UniformBuffer::Create(sizeof(glm::mat4), 1);


		// Default Directional light settings
		data.FrameData.DirLightUBData.Direction = glm::vec4(0.0f, -1.0f, 0.03f, 1.0f);
		data.FrameData.DirLightUBData.Intensity = glm::vec4(1.0f);
		data.FrameData.DirLightUBData.AmbiantFactor = glm::vec4(0.4f);

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

			data.CubeVertexArray = VertexArray::Create();

			VertexBufferLayout vbLayout = { 
				{ soso::ShaderDataType::Float3, "a_Position" } 
			};

			auto vb = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
			vb->SetLayout(vbLayout);
			data.CubeVertexArray->AddVertexBuffer(vb);
			data.CubeVertexArray->SetIndexBuffer(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices)));
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

			data.FSQuadVertexArray = VertexArray::Create();

			VertexBufferLayout vbLayout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			};
			
			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

			auto vb = VertexBuffer::Create(quadData, 4 * sizeof(QuadVertexData));
			vb->SetLayout(vbLayout);

			data.FSQuadVertexArray->AddVertexBuffer(vb);
			data.FSQuadVertexArray->SetIndexBuffer(IndexBuffer::Create(indices, 6 * sizeof(uint32_t)));

			delete[] quadData;
		}


		// Primitive geometry setup
		s_PrimitiveData = new PrimitiveGeometryRendererData;
		s_PrimitiveData->LineVertexArray = VertexArray::Create();
		s_PrimitiveData->LineVertexBuffer = VertexBuffer::Create(s_PrimitiveData->c_MaxLineCount * 2 * sizeof(LineVertex));

		VertexBufferLayout lineLayout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
		};

		s_PrimitiveData->LineVertexBuffer->SetLayout(lineLayout);
		s_PrimitiveData->LineVertexArray->AddVertexBuffer(s_PrimitiveData->LineVertexBuffer);
		s_PrimitiveData->LineVertexBufferBasePtr = std::make_unique<LineVertex[]>(s_PrimitiveData->c_MaxLineCount * 2);
		s_PrimitiveData->LineVertexBufferCurrentPtr = s_PrimitiveData->LineVertexBufferBasePtr.get();

		s_PrimitiveData->LineShader = data.ShaderLib->Get("Line");

	}

	void Renderer::Shutdown() {

		delete s_Data;
		delete s_PrimitiveData;
		delete s_RendererAPI;
	}

	void Renderer::BeginScene(SceneCamera& camera) {

		SS_PROFILE_FUNCTION();
		
		//if (s_Resize) {
		//
		//	s_Data->ShadowPassFrameBuffer->Resize((uint32_t)s_WindowSize.x, (uint32_t)s_WindowSize.y);
		//	s_Data->CompositeFrameBuffer->Resize((uint32_t)s_WindowSize.x, (uint32_t)s_WindowSize.y);
		//	s_Resize = false;
		//}

		s_Stats.DrawCalls = 0;
		s_Stats.Meshes = 0;

		auto& data = *s_Data;
		auto& frameData = data.FrameData;
		auto& lightData = frameData.DirLightUBData;
		auto& cameraData = frameData.CamUBData;
		auto& dirShadowData = frameData.DirShadowMapUBData;

		cameraData.ViewProjection = camera.GetViewProjection();
		cameraData.RotationOnlyViewProjection = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		cameraData.CameraPos = glm::vec4(camera.GetPosition(), 1.0f);

		// Shadow Map 
		glm::vec3 lightDir = glm::normalize(glm::vec3(lightData.Direction));
		glm::vec3 eye = -lightDir * s_RendererSettings.ShadowEyeFactor; // push back so ortho covers scene
		glm::vec3 target = glm::vec3(0.0f);
		auto nearPlane = s_RendererSettings.ShadowNearPlane, farPlane = s_RendererSettings.ShadowFarPlane;
		auto& f = s_RendererSettings.ShadowFrustumBounds;
		auto&& LightProjection = glm::ortho(-f, f, -f, f, nearPlane, farPlane);
		auto&& LightView = glm::lookAt(eye, target + lightDir, glm::vec3(0, 1, 0));
		dirShadowData.LightViewProjection = LightProjection * LightView;

		data.FinalPassMaterial->Set("u_Uniform.Exposure", camera.GetExposure());
		
		// Update per-frame GPU state
		frameData.CameraBufferUBObject->SetData(&cameraData, sizeof(cameraData));
		frameData.LightBufferUBObject->SetData(&lightData, sizeof(DirectionalLightUBData));
		frameData.ShadowLightViewUBObject->SetData(&dirShadowData, sizeof(DirectionalShadowMapUBData));

		BeginPrimitiveGeometryBatch();
	}

	void Renderer::EndScene() {

		SS_PROFILE_FUNCTION();

		s_Stats.LineCount = s_PrimitiveData->LineCount;
		Flush();
	}

	void Renderer::DispatchCompute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {

		SS_PROFILE_FUNCTION();

		s_RendererAPI->DispatchCompute(groupCountX, groupCountY, groupCountZ);
	}

	void Renderer::Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {

		SS_PROFILE_FUNCTION();

		shader->Bind();
		shader->SetUniform("u_Transform", transform);

		s_RendererAPI->DrawIndexed(vertexArray);
		s_Stats.DrawCalls++;
	}

	void Renderer::RenderFullscreenQuad(std::shared_ptr<Material> material) {
		
		SS_PROFILE_FUNCTION();

		auto&& shader = (material) ? material->GetShader() : s_Data->ShaderLib->Get("FSQuad");
		shader->Bind();
		
		if (material) 
			material->Bind();

		s_RendererAPI->DrawIndexed(s_Data->FSQuadVertexArray, 6);
		s_Stats.DrawCalls++;
	}


	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, const std::shared_ptr<Material>& materialOverride) {

		SS_PROFILE_FUNCTION();

		// TODO: batching, culling, etc

		DrawCommand& command = s_Data->DrawList.emplace_back();
		command.pMesh = mesh.get();
		command.Transform = transform;
		command.pMaterialOverride = materialOverride.get();

		s_Stats.DrawCalls += (uint32_t)mesh->GetSubmeshes().size();
		s_Stats.Meshes++;
	}

	void Renderer::SubmitQuad(std::shared_ptr<Material> material, const glm::mat4& transform) {

		SS_CORE_ASSERT(false, "Not implemented");
	}

	void Renderer::SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color) {

		SS_PROFILE_FUNCTION();

		auto& data = *s_PrimitiveData;
		if (data.LineCount >= data.c_MaxLineCount)
			FlushPrimitiveGeometry();

		data.LineVertexBufferCurrentPtr->Position = p0;
		data.LineVertexBufferCurrentPtr->Color = color;
		data.LineVertexBufferCurrentPtr++;

		data.LineVertexBufferCurrentPtr->Position = p1;
		data.LineVertexBufferCurrentPtr->Color = color;
		data.LineVertexBufferCurrentPtr++;

		data.LineCount++;
	}

	void Renderer::SubmitAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color) {

		SS_PROFILE_FUNCTION();

		const glm::vec4 corners[8] = {

			// Bottom Face
			transform * glm::vec4(aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f),
			transform * glm::vec4(aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f),
			transform * glm::vec4(aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f),
			transform * glm::vec4(aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f),

			// Top Face
			transform * glm::vec4(aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f),
			transform * glm::vec4(aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f),
			transform * glm::vec4(aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f),
			transform * glm::vec4(aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f)
		};

		for (uint32_t i = 0; i < 4; i++) {
			SubmitLine(corners[i], corners[(i + 1) % 4], color);
		}

		for (uint32_t i = 0; i < 4; i++) {
			SubmitLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);
		}

		for (uint32_t i = 0; i < 4; i++) {
			SubmitLine(corners[i], corners[i + 4], color);
		}
	}

	void Renderer::BeginPrimitiveGeometryBatch() {

		s_PrimitiveData->LineCount = 0;
		s_PrimitiveData->LineVertexBufferCurrentPtr = s_PrimitiveData->LineVertexBufferBasePtr.get();
	}

	void Renderer::FlushPrimitiveGeometry() {

		SS_PROFILE_FUNCTION();

		auto& pdata = *s_PrimitiveData;
		if (pdata.LineCount == 0)
			return;

		uint32_t vertexCount = pdata.LineCount * 2;

		pdata.LineVertexBuffer->SetData(pdata.LineVertexBufferBasePtr.get(), vertexCount * sizeof(LineVertex));
		pdata.LineShader->Bind();

		s_RendererAPI->DrawArrays(pdata.LineVertexArray, vertexCount);
		s_Stats.DrawCalls++;

		pdata.LineCount = 0;
		pdata.LineVertexBufferCurrentPtr = pdata.LineVertexBufferBasePtr.get();
	}


	void Renderer::Flush() {

		SS_PROFILE_FUNCTION();
		
		auto& data = *s_Data;

		auto transformUB = data.FrameData.TransformUBObject.get();


		if (s_RendererSettings.ShadowPass) {

			SS_PROFILE_SCOPE("Shadow Pass");

			data.ShadowPassFrameBuffer->Bind();
			Clear(1, 0, 1, 1);

			auto shadowMapShader = data.ShadowMapShader.get();

			for (auto& dc : data.DrawList) {

				s_RendererAPI->DrawMesh(dc.pMesh, transformUB, dc.Transform, dc.pMaterialOverride, shadowMapShader);
			}

			data.ShadowPassFrameBuffer->Unbind();

		}

		data.GeometryFrameBuffer->Bind();
		{
			Clear(0.2f, 0.2f, 0.2f, 1.0f);

			// skybox pass
			if (s_RendererSettings.SkyboxPass) {
				
				SS_PROFILE_SCOPE("Skybox Pass");

				s_RendererAPI->DrawSkybox(data.SceneData.SkyboxMaterial, data.CubeVertexArray);
				s_Stats.DrawCalls++;
			}

			// Opaque geometry pass
			{
				SS_PROFILE_SCOPE("Opaque Pass");

				glBindTextureUnit(7, s_RendererSettings.ShadowPass ? data.ShadowPassFrameBuffer->GetDepthAttachmentRendererID() : data.WhiteTexture->GetTextureHandle());
				data.SceneData.EnviromentMaterial->Bind();
				for (auto& dc : data.DrawList) {
					s_RendererAPI->DrawMesh(dc.pMesh, transformUB, dc.Transform, dc.pMaterialOverride);
				}
			}


			// Wireframe pass

			if (s_RendererSettings.WirePass) {

				SS_PROFILE_SCOPE("Wire Pass");

				for (auto& dc : data.DrawList) {

					s_RendererAPI->DrawMesh(dc.pMesh, transformUB, dc.Transform, dc.pMaterialOverride, data.ShaderLib->Get("MeshWireframe").get());
				}
			}

			// Render Primitive Geometry
			FlushPrimitiveGeometry();

		}
		data.GeometryFrameBuffer->Unbind();


		data.CompositeFrameBuffer->Bind();
		{
			Clear(0.2f, 0.2f, 0.2f, 1.0f);
		
		
			glBindTextureUnit(0, data.GeometryFrameBuffer->GetColorAttachmentRendererID());
			RenderFullscreenQuad(data.FinalPassMaterial);
		
		}
		data.CompositeFrameBuffer->Unbind();

		data.DrawList.clear();
	}

	//=============================================================================================================================

	Environment Renderer::CreateEnvironment(std::filesystem::path equirectangularHDRTexture) {

		SS_PROFILE_FUNCTION();

		auto& data = *s_Data;

		auto inputTex = Texture2D::Create(TextureConfig(), equirectangularHDRTexture);
		SS_CORE_ASSERT(inputTex->GetFormat() == ImageFormat::RGBA32F, "Must pass in HDR image file");

		// Convert equirectangular HDR texture to textureCube
		TextureConfig unfilteredConfig;
		unfilteredConfig.Format = ImageFormat::RGBA16F;
		unfilteredConfig.Width = 1024;
		unfilteredConfig.Height = 1024;
		unfilteredConfig.GenerateMips = true;
		auto unfilteredTex = TextureCube::Create(unfilteredConfig);
		
		auto compute = data.ShaderLib->Get("EquirectangularToCubeCompute");

		compute->Bind();
		inputTex->Bind(0);
		glBindImageTexture(1, unfilteredTex->GetTextureHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		DispatchCompute(unfilteredTex->GetWidth() / 32, unfilteredTex->GetHeight() / 32, 6);
		unfilteredTex->GenerateMips();

		const uint32_t cubemapResolution = 32;

		// Compute irradiance map
		TextureConfig irradianceConfig;
		irradianceConfig.Format = ImageFormat::RGBA16F;
		irradianceConfig.Width = cubemapResolution;
		irradianceConfig.Height = cubemapResolution;
		irradianceConfig.GenerateMips = true;
		
		auto irradianceTex = TextureCube::Create(irradianceConfig);

		compute = data.ShaderLib->Get("IrradianceMapCompute");
		compute->Bind();
		unfilteredTex->Bind(0);
		glBindImageTexture(1, irradianceTex->GetTextureHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		DispatchCompute(irradianceTex->GetWidth() / 32, irradianceTex->GetHeight() / 32, 6);

		irradianceTex->GenerateMips();

		// Compute radiance map at each mip level
		TextureConfig radianceConfig;
		radianceConfig.Format = ImageFormat::RGBA16F;
		radianceConfig.Width = cubemapResolution;
		radianceConfig.Height = cubemapResolution;
		radianceConfig.GenerateMips = true;

		auto radianceTex = TextureCube::Create(radianceConfig);

		compute = data.ShaderLib->Get("RadianceMapCompute");

		const uint32_t numMipLevels = radianceTex->GetNumMipLevels();
		const float deltaRoughness = 1.0f / (numMipLevels - 1.0f);

		compute->Bind();
		unfilteredTex->Bind(0);

		for (uint32_t i = 0, size = cubemapResolution; i < numMipLevels; i++, size = size / 2) {

			uint32_t groupCount = glm::max(1u, size / 32);
			float roughness = i * deltaRoughness;
			roughness = glm::max(roughness, 0.05f);	

			glBindImageTexture(1, radianceTex->GetTextureHandle(), i, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			compute->SetUniform("u_Roughness", roughness);

			DispatchCompute(groupCount, groupCount, 6);
		}
		
		Environment env;
		env.UnfilteredMap = unfilteredTex;
		env.RadianceMap = radianceTex;
		env.IrradianceMap = irradianceTex;
		return env;
		
	}

	void Renderer::SetSkyboxTexture(std::shared_ptr<TextureCube> texture) {

		s_Data->SceneData.SkyboxMaterial->Set("u_Skybox", texture);
	}

	void Renderer::SetEnvironment(const Environment environment) {

		auto& sceneData = s_Data->SceneData;

		sceneData.Enviroment = environment;
		sceneData.EnviromentMaterial->Set("u_EnvIrradianceTexture", environment.IrradianceMap);
		sceneData.EnviromentMaterial->Set("u_EnvRadianceTexture", environment.RadianceMap);
	}

	std::shared_ptr<ShaderLibrary> Renderer::GetShaderLibrary() {

		return s_Data->ShaderLib;
	}

	std::shared_ptr<FrameBuffer> Renderer::GetCompositeFrameBuffer() {

		return s_Data->CompositeFrameBuffer;
	}

	std::shared_ptr<Texture2D> Renderer::GetWhiteTexture() {

		return s_Data->WhiteTexture;
	}

	uint32_t Renderer::GetCurrentFrameIndex() {

		return Application::Get().GetCurrentFrameIndex();
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

		SS_PROFILE_FUNCTION();

		auto& frameData = s_Data->FrameData;

		static bool showShadowMap = false;

		ImGui::Begin("Renderer Settings");
		{

			// --- Render Passes ---
			if (ImGui::CollapsingHeader("Render Passes"))
			{
				UI::ToggleSwitch("Shadow Pass", &s_RendererSettings.ShadowPass);
				UI::ToggleSwitch("Skybox Pass", &s_RendererSettings.SkyboxPass);
				UI::ToggleSwitch("Mesh Wireframe Pass", &s_RendererSettings.WirePass);
			}

			// ---- Directional Light ----
			if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SliderFloat3("Direction", &frameData.DirLightUBData.Direction.x, -1.0f, 1.0f);
				ImGui::SliderFloat("Intensity", &frameData.DirLightUBData.Intensity.x, 0.0f, 5.0f);
				frameData.DirLightUBData.Intensity = glm::vec4(glm::vec3(frameData.DirLightUBData.Intensity.x), 1.0f);
				ImGui::SliderFloat("Ambient", &frameData.DirLightUBData.AmbiantFactor.x, 0.0f, 5.0f);
				frameData.DirLightUBData.AmbiantFactor = glm::vec4(glm::vec3(frameData.DirLightUBData.AmbiantFactor.x), 1.0f);
			}

			// ---- Shadow Map ----
			if (ImGui::CollapsingHeader("Shadow Map Settings"))
			{
				UI::ToggleSwitch("View Shadow Map", &showShadowMap);
				ImGui::SliderFloat("Shadow Frustum Bounds", &s_RendererSettings.ShadowFrustumBounds, 1.0f, 100.0f);
				ImGui::SliderFloat("Shadow Near Plane", &s_RendererSettings.ShadowNearPlane, 0.001f, 15.0f);
				ImGui::SliderFloat("Shadow Far Plane", &s_RendererSettings.ShadowFarPlane, 1.0f, 400.0f);
				ImGui::SliderFloat("Shadow Eye Factor", &s_RendererSettings.ShadowEyeFactor, 1.0f, 200.0f);
			}

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
