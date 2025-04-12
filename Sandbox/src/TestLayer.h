#pragma once
#include <soso.h>

#include <so-so/Asset/TextureImporter.h>
#include "so-so/Renderer/Mesh.h"
#include "Misc/Skybox.h"

#include "glm/ext.hpp"
#include "imgui.h"

struct Transform {
	glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

	glm::mat4 GetMatrix() const {
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, Position);
		matrix = glm::rotate(matrix, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, Scale);
		return matrix;
	}
};

class TestLayer : public soso::Layer {
public:

	TestLayer()
		:Layer("Test") 
	{
		
	}

	void OnAttach() override {

		SS_INFO("Current working directory: {0}", std::filesystem::current_path().string());

		float width = (float)soso::Application::Get().GetWindow().GetWidth(),
			height = (float)soso::Application::Get().GetWindow().GetHeight();
		m_Camera = soso::SceneCamera(80.0, width, height, 0.1f, 100000.0f);

		
		m_Mesh = std::make_shared<soso::Mesh>("assets/backpack/backpack.obj");
		
		m_Mesh->DumpBufferInfo();

		m_TexCube = soso::TextureImporter::LoadTextureCube({
			"assets/textures/Skybox/skybox/right.jpg",
			"assets/textures/Skybox/skybox/left.jpg",
			"assets/textures/Skybox/skybox/top.jpg",
			"assets/textures/Skybox/skybox/bottom.jpg",
			"assets/textures/Skybox/skybox/front.jpg",
			"assets/textures/Skybox/skybox/back.jpg"
		});

		m_Skybox = Skybox(m_TexCube);

		//m_MeshTransform.Scale *= 0.1;
	}

	void OnUpdate(soso::Timestep ts) override {

		m_TotalTime += ts.GetSeconds();
		
		m_Camera.OnUpdate(ts);

		soso::Renderer::BeginScene(m_Camera);
		{

			soso::Renderer::SubmitMesh(m_Mesh, m_MeshTransform.GetMatrix());
			m_Skybox.Draw();
			
		}
		soso::Renderer::EndScene();
	}

	void OnEvent(soso::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void OnImGuiRender() override {
		
		ImGui::Begin("Data");
		{

			ImGui::Begin("Transform Editor");

			ImGui::DragFloat3("Position", &m_MeshTransform.Position.x, 0.1f);
			ImGui::DragFloat3("Scale", &m_MeshTransform.Scale.x,    0.05f, 0.0f, 10.0f);

			ImGui::DragFloat3("Rotation", &m_MeshTransform.Rotation.x, 1.0f, -360.0f, 360.0f, "%.1f°");

			if (ImGui::Button("Reset Rotation"))
				m_MeshTransform = Transform();

			ImGui::End();

			soso::Renderer::ImGuiRendererDebug();
		}
		ImGui::End();
	}


private:

	float m_TotalTime = 0.0f;

	soso::SceneCamera m_Camera;

	Transform m_MeshTransform{};
	std::shared_ptr<soso::Mesh> m_Mesh;

	std::shared_ptr<soso::TextureCube> m_TexCube;
	Skybox m_Skybox;

private:
	
	int m_TileScale{ 1 };
	//---------------------------------------------
	soso::ShaderLibrary m_ShaderLibrary;
};