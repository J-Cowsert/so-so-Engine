#pragma once

#include "glm/glm.hpp"

#include "ShaderLibrary.h"

#include "RendererAPI.h"

namespace soso {

	class Mesh;
	class Material;
	class Texture2D;
	class SceneCamera;
	class FrameBuffer;

	struct Statistics {

		uint32_t DrawCalls = 0;
		uint32_t Meshes = 0;
	};

	// This probably shouldn't be static in the future. It could be useful to have multiple instances
	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void Clear(float r, float g, float b, float a);

		static std::shared_ptr<ShaderLibrary> GetShaderLibrary();
		static std::shared_ptr<FrameBuffer> GetCompositeFrameBuffer();
		static std::shared_ptr<Texture2D> GetWhiteTexture(); // TODO: Test this 
		
		static const Statistics& GetStats() { return s_Stats; }

		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void RenderFullscreenQuad();

		static void SetSkyboxTexture(std::shared_ptr<TextureCube> texture);

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr);

		// Primitive Geometry - TODO
		static void SubmitQuad(std::shared_ptr<Material> material, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitBillboardQuad() {}
		static void SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0));

		static void ImGuiRendererDebug(); // TODO: Rename

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		static void ShadowPass();

		static void FlushDrawList();

	private:
		struct CameraUBData {
			glm::mat4 ViewProjection;
			glm::mat4 RotationOnlyViewProjection;;
			glm::vec4 CameraPos;
		};

		static_assert(sizeof(CameraUBData) % 16 == 0, "Size must be a multiple of 16 bytes");

		struct DirectionalLightUBData {
			glm::vec4 Direction;
			glm::vec4 Ambient;
			glm::vec4 Diffuse;
			glm::vec4 Specular;
		};

		static_assert(sizeof(DirectionalLightUBData) % 16 == 0, "Size must be a multiple of 16 bytes");

		struct DirectionalShadowMapUBData {;
			glm::mat4 LightViewProjection;
		};

		static_assert(sizeof(DirectionalShadowMapUBData) % 16 == 0, "Size must be a multiple of 16 bytes");

		struct FrameData {
			DirectionalShadowMapUBData DirShadowMapUBData;
			CameraUBData CameraUBData;
			DirectionalLightUBData DirLightUBData;
		};

		struct DrawCommand {
			glm::mat4 Transform;
			std::shared_ptr<Mesh> Mesh;
			std::shared_ptr<Material> MaterialOverride;
		};

		struct QuadCommand {

			std::shared_ptr<Material> Material;
			glm::mat4 Transform;
		};


		// =============


		struct SceneRendererData {

			FrameData FrameData;
			
			std::vector<DrawCommand> DrawList;
			std::vector<QuadCommand> QDrawList;

			std::shared_ptr<FrameBuffer> CompositeFrameBuffer;
			std::shared_ptr<FrameBuffer> ShadowPassFrameBuffer;
			std::shared_ptr<Shader> ShadowMapShader;

			std::shared_ptr<UniformBuffer> CameraBufferUBObject;
			std::shared_ptr<UniformBuffer> LightBufferUBObject;
			std::shared_ptr<UniformBuffer> ShadowLightViewUBObject;
			std::shared_ptr<UniformBuffer> TransformBufferUBObject;

			std::shared_ptr<VertexArray> CubeVertexArray;
			std::shared_ptr<VertexArray> FSQuadVertexArray; // Move to primative geometry data and rename

			std::shared_ptr<TextureCube> SkyboxTexture;
			std::shared_ptr<Material> SkyboxMaterial;

			std::shared_ptr<Texture2D> WhiteTexture;
			std::shared_ptr<Texture2D> GreyTexture; // TODO
			std::shared_ptr<Texture2D> BlackTexture; // TODO

			std::shared_ptr<ShaderLibrary> ShaderLibrary;
		};

		inline static SceneRendererData* s_Data = nullptr;

	private:
		// TODO
		void BeginPrimitiveGeometryBatch();
		void FlushPrimitiveGeometry();

		struct LineVertex {
			glm::vec3 Position;
			glm::vec4 Color;
		};


		struct PrimitiveGeometryRendererData {

			const uint32_t c_MaxLineCount = 5000;
			std::shared_ptr<VertexArray> m_LineVertexArray;
			std::shared_ptr<VertexBuffer> m_LineVertexBuffer;

			std::unique_ptr<LineVertex[]> m_LineVertexBufferBasePtr;
			std::unique_ptr<LineVertex[]> m_LineVertexBufferCurrentPtr;
		};

	private:

		static Statistics s_Stats;

		inline static glm::vec2 s_WindowSize{ 0, 0 };
		inline static bool s_Resize = false;
	};
}