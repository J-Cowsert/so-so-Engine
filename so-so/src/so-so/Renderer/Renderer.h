#pragma once

#include "glm/glm.hpp"
#include "RenderCommand.h"
#include "ShaderLibrary.h"

namespace soso {

	class Mesh;
	class Material;
	class SceneCamera;
	class FrameBuffer;
	class Texture2D;

	class Renderer {
	public:
		struct Statistics {
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
			uint32_t Instances = 0;
			uint32_t SavedDraws = 0;

			float TotalGPUTime = 0.0f;
		};

	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static std::shared_ptr<ShaderLibrary> GetShaderLibrary();
		static std::shared_ptr<FrameBuffer> GetCompositeFrameBuffer();
		static std::shared_ptr<Texture2D> GetWhiteTexture();
		
		// static const Statistics& GetStats() { return s_Stats; }

		static void BeginScene(SceneCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr);
		
		static void SubmitSkybox(const std::shared_ptr<Shader> shader, const std::shared_ptr<VertexArray>& vertexArray); // TODO: change

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void ImGuiRendererDebug();

	private:
		static Statistics s_Stats;
	};
}