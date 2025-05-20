#pragma once

#include "Renderer.h"
#include "SceneCamera.h"


namespace soso {

	struct SceneRendererStatistics {


	};
	
	class SceneRenderer {
	public:
		void Init();
		void Shutdown();

		void BeginScene(SceneCamera& camera);
		void EndScene();

		void SubmitMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const std::shared_ptr<Material>& materialOverride = nullptr);


	private:
		

	private:
		struct CameraUBData {
			glm::mat4 ViewProjection;
			glm::mat4 InverseViewProjection;
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

		struct SceneData {

			std::shared_ptr<FrameBuffer> CompositeFrameBuffer;
			std::shared_ptr<FrameBuffer> ShadowPassFrameBuffer;

			std::shared_ptr<UniformBuffer> CameraBufferObject;
			std::shared_ptr<UniformBuffer> LightBufferObject;
			std::shared_ptr<UniformBuffer> TransformBufferObject;
		};
	};
}