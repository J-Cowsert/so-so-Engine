#include "sspch.h"
#include "OpenGLRenderer.h"

#include "glad/glad.h"

namespace soso {

	namespace Utils {

		static void OpenGLDebugCallback(
			unsigned source,
			unsigned type,
			unsigned id,
			unsigned severity,
			int length,
			const char* message,
			const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:         SS_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       SS_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          SS_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: SS_CORE_TRACE(message); return;
			}

			SS_CORE_ASSERT(false, "Unknown severity level!");
		}

		static GLenum PrimativeTypeToGLType(PrimitiveType type) {

			switch (type) {

			case PrimitiveType::Triangles: return GL_TRIANGLES;
			case PrimitiveType::Lines:     return GL_LINES;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}
	}

	void OpenGLRenderer::Init() {

	#ifdef SS_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Utils::OpenGLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		//glEnable(GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		//glEnable(GL_FRAMEBUFFER_SRGB);

		SetClearColor(1, 0, 1, 1);

		GLenum error = glGetError();
		while (error != GL_NO_ERROR) {
			SS_CORE_ERROR("OpenGL Error {0}", error);
			error = glGetError();
		}

		glUseProgram(0);
	}

	void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRenderer::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGLRenderer::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRenderer::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, const uint32_t indexCount, PrimitiveType type /* = PrimitiveType::Triangles */) {

		vertexArray->Bind();
		glDrawElements(Utils::PrimativeTypeToGLType(type), (indexCount) ? indexCount : vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
		
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRenderer::DrawSkybox(const std::shared_ptr<Material> material, const std::shared_ptr<VertexArray>& vertexArray) {

		auto&& shader = material->GetShader();

		shader->Bind();

		material->Bind();

		glDepthFunc(GL_LEQUAL);
		DrawIndexed(vertexArray);
		glDepthFunc(GL_LESS);
	}

	void OpenGLRenderer::DrawMesh(std::shared_ptr<Mesh>& mesh, std::shared_ptr<UniformBuffer>& transformUB, const glm::mat4& transform, const std::shared_ptr<Material>& materialOverride, const std::shared_ptr<Shader>& shaderOverride) {
		
		mesh->m_VertexArray->Bind();

		const auto& shader = (shaderOverride) ? shaderOverride : mesh->m_DefaultShader;
		const auto& materials = mesh->m_Materials;

		// Tracks last bound material to skip redundant Bind() calls
		const Material* lastUsedMat = nullptr;

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (auto&& submesh : mesh->m_Submeshes) {

			const auto& material = (materialOverride) ? materialOverride : materials[submesh.MaterialIndex];

			//if (shader)
			//	shader->Bind();
			//else
			//	material->GetShader()->Bind();

			shader->Bind();

			if (material.get() != lastUsedMat) { material->Bind(); lastUsedMat = material.get(); }

			glm::mat4 localTransform = transform * submesh.Transform;

			transformUB->SetData(&localTransform, sizeof(localTransform));

			glDrawElementsBaseVertex(GL_TRIANGLES,
				submesh.IndexCount,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint32_t) * submesh.BaseIndex),
				submesh.BaseVertex);

			//s_Stats.DrawCalls++;
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

	

	void OpenGLRenderer::SetLineWidth(float width) {
		glLineWidth(width);
	}
}