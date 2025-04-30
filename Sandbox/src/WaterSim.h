#pragma once

#pragma once
#include <soso.h>

#include <so-so/Asset/TextureImporter.h>
#include "so-so/Renderer/MeshGenerator.h"
#include "Misc/Skybox.h"

#include "glm/ext.hpp"
#include "imgui.h"



class WaterSim : public soso::Layer {
public:

	WaterSim()
		:Layer("Test") {

	}

	void OnAttach() override {

		float width = (float)soso::Application::Get().GetWindow().GetWidth(),
			  height = (float)soso::Application::Get().GetWindow().GetHeight();
		m_Camera = soso::SceneCamera(80.0, width, height, 0.1f, 100000.0f);


		m_Mesh = soso::Mesh::Create("assets/backpack/backpack.obj");

		m_Mesh->DumpBufferInfo();

		//m_TexCube = soso::TextureImporter::LoadTextureCube({
		//	"assets/textures/Skybox/skybox/right.jpg",
		//	"assets/textures/Skybox/skybox/left.jpg",
		//	"assets/textures/Skybox/skybox/top.jpg",
		//	"assets/textures/Skybox/skybox/bottom.jpg",
		//	"assets/textures/Skybox/skybox/front.jpg",
		//	"assets/textures/Skybox/skybox/back.jpg"
		//	});
		//
		//m_Skybox = Skybox(m_TexCube);
	}

	void OnUpdate(soso::Timestep ts) override {

		m_TotalTime += ts.GetSeconds();

		m_Camera.OnUpdate(ts);

		soso::Renderer::BeginScene(m_Camera);
		{
			//soso::Renderer::SubmitMesh(m_Mesh, );
		}
		soso::Renderer::EndScene();
	}

	void OnEvent(soso::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void OnImGuiRender() override {

		ImGui::Begin("Data");
		{

			soso::Renderer::ImGuiRendererDebug();
		}
		ImGui::End();
	}


private:

	float m_TotalTime = 0.0f;

	soso::SceneCamera m_Camera;

	std::shared_ptr<soso::Mesh> m_Mesh;

	std::shared_ptr<soso::TextureCube> m_TexCube;
	Skybox m_Skybox;

private:

	int m_TileScale{ 1 };
};