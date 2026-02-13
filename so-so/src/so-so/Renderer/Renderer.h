#pragma once

#include "RendererAPI.h"
#include "SceneCamera.h"
#include "UniformBufferRing.h"
#include "Math/AABB.h"

#include "glm/glm.hpp"

namespace soso {

	class Mesh;
	class Material;
	class Texture2D;
	class FrameBuffer;
	class ShaderLibrary;

	struct RendererConfig {

		uint32_t MaxFramesInFlight = 2;
	};

	struct Statistics {

		uint32_t DrawCalls = 0;
		uint32_t Meshes = 0;
		uint32_t Submeshes = 0;
		uint32_t LineCount = 0;
	};

	struct Environment {

		std::shared_ptr<TextureCube> UnfilteredMap;
		std::shared_ptr<TextureCube> RadianceMap;
		std::shared_ptr<TextureCube> IrradianceMap;
	};

	class Renderer {
	public:
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height); // Investigate
		static void Clear(float r, float g, float b, float a);

		static Environment CreateEnvironment(std::filesystem::path equirectangularHDRTexture);
		static void SetLineWidth(float width);
		static void SetSkyboxTexture(std::shared_ptr<TextureCube> texture);
		static void SetEnvironment(const Environment environment);

		static std::shared_ptr<ShaderLibrary> GetShaderLibrary();
		static std::shared_ptr<FrameBuffer> GetCompositeFrameBuffer();
		static std::shared_ptr<Texture2D> GetWhiteTexture(); 
		static uint32_t GetCurrentFrameIndex();
		
		static const Statistics& GetStats() { return s_Stats; }
		static RendererConfig GetConfig() { return s_Config; }
		
		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void DispatchCompute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
		static void RenderFullscreenQuad(std::shared_ptr<Material> material = nullptr);

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		
		static void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr);
		static void SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0f));
		static void SubmitAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
		static void SubmitQuad(std::shared_ptr<Material> material, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitBillboardQuad() {} // TODO:


		static void ImGuiRendererDebug();


	private:
		//static void ShadowPass();
		//static void GeometryPass();
		//static void CompositePass();

		static void Flush();

	private:

		struct CameraUBData {

			glm::mat4 ViewProjection;
			glm::mat4 RotationOnlyViewProjection;;
			glm::vec4 CameraPos;
		};

		static_assert(sizeof(CameraUBData) % 16 == 0, "Size must be a multiple of 16 bytes");

		struct DirectionalLightUBData {

			glm::vec4 Direction;
			glm::vec4 Intensity; 
			glm::vec4 AmbiantFactor; // temp 
		};

		static_assert(sizeof(DirectionalLightUBData) % 16 == 0, "Size must be a multiple of 16 bytes");

		struct DirectionalShadowMapUBData {

			glm::mat4 LightViewProjection;
		};

		static_assert(sizeof(DirectionalShadowMapUBData) % 16 == 0, "Size must be a multiple of 16 bytes");


		//====================================

		struct TransformVertexData { // TODO

			glm::vec4 MRow[3];
		};

		struct DrawCommand {

			Mesh* pMesh;
			Material* pMaterialOverride;
			glm::mat4 Transform;
			uint32_t SubmeshIndex;
		};

		struct PerSceneData {

			Environment Enviroment;
			std::shared_ptr<Material> EnviromentMaterial;
			std::shared_ptr<Material> SkyboxMaterial;
		};

		struct PerFrameData {

			CameraUBData CamUBData;
			DirectionalLightUBData DirLightUBData;
			DirectionalShadowMapUBData DirShadowMapUBData;

			std::shared_ptr<UniformBuffer> CameraBufferUBObject;
			std::shared_ptr<UniformBuffer> LightBufferUBObject;
			std::shared_ptr<UniformBuffer> ShadowLightViewUBObject;

			std::shared_ptr<UniformBuffer> TransformUBObject; // Very high frequency
		};


		struct RendererData {

			PerFrameData FrameData;
			PerSceneData SceneData;
			
			std::vector<DrawCommand> DrawList;

			std::shared_ptr<ShaderLibrary> ShaderLib;

			std::shared_ptr<FrameBuffer> CompositeFrameBuffer;
			std::shared_ptr<FrameBuffer> GeometryFrameBuffer;
			std::shared_ptr<FrameBuffer> ShadowPassFrameBuffer;

			std::shared_ptr<Shader> ShadowMapShader;
			std::shared_ptr<Shader> FinalPassShader;

			std::shared_ptr<Material> FinalPassMaterial;

			std::shared_ptr<VertexArray> CubeVertexArray;
			std::shared_ptr<VertexArray> FSQuadVertexArray; // Move to primative geometry data

			std::shared_ptr<Texture2D> WhiteTexture;
			std::shared_ptr<Texture2D> GreyTexture; // TODO
			std::shared_ptr<Texture2D> BlackTexture; // TODO
		};

		inline static RendererData* s_Data = nullptr;


	private:
		static void BeginPrimitiveGeometryBatch();
		static void FlushPrimitiveGeometry();

		struct LineVertex {

			glm::vec3 Position;
			glm::vec4 Color;
		};
		
		struct PrimitiveGeometryRendererData {

			const uint32_t c_MaxLineCount = 100000;
			
			std::shared_ptr<VertexArray> LineVertexArray;
			std::shared_ptr<VertexBuffer> LineVertexBuffer;
			std::shared_ptr<Shader> LineShader;

			std::unique_ptr<LineVertex[]> LineVertexBufferBasePtr;
			LineVertex* LineVertexBufferCurrentPtr = nullptr;

			uint32_t LineCount = 0;
		};

		inline static PrimitiveGeometryRendererData* s_PrimitiveData = nullptr;
	
private:

		inline static RendererConfig s_Config;

		struct RendererSettings {
			
			bool SkyboxPass = true;

			bool ShadowPass = false;
			bool WirePass = false;

			float ShadowFrustumBounds = 50.0f;
			float ShadowNearPlane = 0.001f, ShadowFarPlane = 50.0f;
			float ShadowEyeFactor = 20.0f;
		};

		

		inline static Statistics s_Stats;

		inline static glm::vec2 s_WindowSize{ 0, 0 };
		inline static bool s_Resize = false;
	};
}
