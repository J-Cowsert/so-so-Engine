#pragma once

#include <soso.h>
#include <so-so/Renderer/Mesh.h>

#include <so-so/Asset/TextureImporter.h>
#include "Misc/Model.h"
#include "Misc/Skybox.h"

#include "glm/ext.hpp"
#include "imgui.h"


class TestLayer : public soso::Layer {
public:
	
	TestLayer()
		:Layer("Test") 
	{

		SS_INFO("Current working directory: {0}", std::filesystem::current_path().string());

		float aspect = (float)soso::Application::Get().GetWindow().GetWidth() / (float)soso::Application::Get().GetWindow().GetHeight();
		m_Camera = soso::SceneCamera(80.0, aspect, 0.1f, 10000.0f);

		m_TexCube = soso::TextureImporter::LoadTextureCube({
			"assets/textures/Skybox/skybox/right.jpg",
			"assets/textures/Skybox/skybox/left.jpg",
			"assets/textures/Skybox/skybox/top.jpg",
			"assets/textures/Skybox/skybox/bottom.jpg",
			"assets/textures/Skybox/skybox/front.jpg",
			"assets/textures/Skybox/skybox/back.jpg"
		});	

		
		m_Skybox = Skybox(m_TexCube);

		auto mesh = soso::Mesh("assets/models/sphere.obj");
		auto tex = soso::TextureImporter::LoadTexture2D("assets/textures/water.png");
		
		m_ShaderLibrary.Load("Phong", "assets/shaders/Phong.glsl");
		m_Shader = m_ShaderLibrary.Get("Phong");

		m_Models.reserve(10);

		for (int i = 0; i < 10; i++)
			m_Models.emplace_back(Model(m_Shader, mesh, tex));

		for (size_t i = 0; i < m_Models.size(); i++) {
			
			m_Models[i].GetTransform().Position = glm::vec3(i * 50, 0, 0);
		}
		m_LightPos.y = 100.0f;
		
	}

	void OnUpdate(soso::Timestep ts) override {

		m_TotalTime += ts.GetSeconds();
		
		m_Camera.OnUpdate(ts);

		soso::Renderer::BeginScene(m_Camera);
		{

			m_LightPos.x = 100 * cos(m_TotalTime);
			m_LightPos.z = 100 * sin(m_TotalTime);

			m_Skybox.Draw();

			for (size_t i = 0; i < m_Models.size(); i++) {

				m_Models[i].GetTransform().Position.y = 50 * cos((m_TotalTime * (i + 1)) / 10);

				m_Models[i].GetTransform().Position.z = 50 * sin((m_TotalTime * (i + 1)) / 10);
				
			}

			for (auto& model : m_Models) {
				model.Draw();
			}
			
			m_Shader->SetFloat("u_Time", m_TotalTime);

			m_Shader->SetFloat3("u_LightPos", m_LightPos);
			m_Shader->SetFloat3("u_ViewPos", m_Camera.GetPosition());
			m_Shader->SetFloat3("u_LightColor", m_LightColor);
			m_Shader->SetFloat("u_AmbientStrength", m_AmbientStrength);
			m_Shader->SetFloat("u_SpecularStrength", m_SpecularStrength);
			m_Shader->SetFloat("u_SpecularShininess", m_SpecularShininess);
			m_Shader->SetFloat("u_LightIntensity", m_LightIntensity);
			m_Shader->SetFloat("u_TileScale", m_TileScale);
		}
		soso::Renderer::EndScene();
	}

	void OnEvent(soso::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void OnImGuiRender() override {
		static bool show = true;

		//ImGui::ShowDemoWindow(&show);
		{
			ImGui::Begin("Debug");

			if (ImGui::CollapsingHeader("Model Parameters")) {
				// Transform Controls
				///~ImGui::Text("Transform");
				///~ImGui::DragFloat3("Position", &m_Transform.Position.x, 0.1f);
				///~ImGui::DragFloat3("Rotation", &m_Transform.Rotation.x, 1.0f, -360.0f, 360.0f);
				///~ImGui::DragFloat3("Scale", &m_Transform.Scale.x, 0.1f, 0.0f, 100.0f);

				ImGui::Separator();

				// Light Controls
				ImGui::DragFloat3("Light Position", &m_LightPos.x, 0.1f);
				ImGui::ColorEdit3("Light Color", &m_LightColor.x);
				ImGui::DragFloat("Ambient Strength", &m_AmbientStrength, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Specular Strength", &m_SpecularStrength, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Specular Shininess", &m_SpecularShininess, 0.1f, 1.0f, 128.0f);
				ImGui::DragFloat("Light Intensity", &m_LightIntensity, 0.1f, 0.0f, 10.0f);
				ImGui::DragInt("Tile Scale", &m_TileScale, 1, 1, 100);
			}


			ImGui::End();
		}
	}


private:

	float m_TotalTime = 0.0f;

	soso::SceneCamera m_Camera;

	std::shared_ptr<soso::TextureCube> m_TexCube;
	Skybox m_Skybox;

private:
	glm::vec3 m_LightPos{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_LightColor{ 1.0f, 1.0f, 1.0f };
	float m_AmbientStrength{ 0.5f };
	float m_SpecularStrength{ 0.5f };
	float m_SpecularShininess{ 32.0f };
	float m_LightIntensity{ 1.0f };
	int m_TileScale{ 1 };
	//---------------------------------------------
	soso::ShaderLibrary m_ShaderLibrary;
	std::shared_ptr<soso::Shader> m_Shader;
	soso::Mesh m_Mesh;
	std::vector<Model> m_Models;
};