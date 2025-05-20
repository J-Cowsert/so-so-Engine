#pragma once

#include "glm/glm.hpp"
#include "ShaderLibrary.h"

#include "RendererAPI.h"

namespace soso {

	class Mesh;
	class Material;
	class SceneCamera;
	class FrameBuffer;
	class Texture2D;

	struct Statistics {

		uint32_t DrawCalls = 0;
		uint32_t Meshes = 0;
	};

	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height); // TODO: Find this method another home
		static void Clear(float r, float g, float b, float a); // TODO: Find this method another home

		static std::shared_ptr<ShaderLibrary> GetShaderLibrary();
		static std::shared_ptr<FrameBuffer> GetCompositeFrameBuffer();
		static std::shared_ptr<Texture2D> GetWhiteTexture(); // TODO: Test this
		
		static const Statistics& GetStats() { return s_Stats; }

		static void BeginRenderPass(std::shared_ptr<FrameBuffer> target);
		static void EndRenderPass();

		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitFullscreenQuad();

		static void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr);
		static void SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray); // TODO: change
		static void SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0)); // TODO

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void ImGuiRendererDebug();

	private:
		// TODO
		void BeginPrimitiveGeometryBatch();
		void FlushPrimitiveGeometry();

		struct LineVertex {
			glm::vec3 Position;
			glm::vec4 Color;
		};

		// Should prolly pull this stuff in RenderData
		const uint32_t c_MaxLineCount = 5000;
		std::shared_ptr<VertexArray> m_LineVertexArray;
		std::shared_ptr<VertexBuffer> m_LineVertexBuffer;

		std::unique_ptr<LineVertex[]> m_LineVertexBufferBasePtr;
		std::unique_ptr<LineVertex[]> m_LineVertexBufferCurrentPtr;

	private:

		static Statistics s_Stats;
	};
}