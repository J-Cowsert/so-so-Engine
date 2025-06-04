#pragma once
#include <soso.h>

#include "so-so/Core/Timer.h"
#include "so-so/ImGui/ImGuiWidgets.h"

#include "Misc/Terrain.h"

#include "glm/ext.hpp"
#include "imgui.h"

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

		m_Camera = soso::SceneCamera(80.0, width, height, 0.1f, 10000.0f);
		m_ShaderLibrary = soso::Renderer::GetShaderLibrary();

		m_TexCube = soso::TextureImporter::LoadTextureCube({
			"assets/textures/Skybox/skybox/right.jpg",
			"assets/textures/Skybox/skybox/left.jpg",
			"assets/textures/Skybox/skybox/top.jpg",
			"assets/textures/Skybox/skybox/bottom.jpg",
			"assets/textures/Skybox/skybox/front.jpg",
			"assets/textures/Skybox/skybox/back.jpg"
			});
		//m_Skybox = Skybox(m_TexCube);

		soso::Renderer::SetSkyboxTexture(m_TexCube);

		m_DefaultMesh = soso::Mesh::Create("assets/the_forgotten_knight/scene.gltf");
		m_DebugMaterial = soso::Material::Create(m_ShaderLibrary->Get("PBR"));

		// Plane
		{	
			Entity& ent = m_Plane;

			ent.Name = "Plane";

			ent.Mesh = soso::MeshGenerator::GeneratePlane(1, 2);

			ent.Transform.Position.y -= 1.8f;
			ent.Transform.Scale.x *= 2;
			ent.Transform.Scale.z *= 2;

			ent.OverrideMaterial = m_DebugMaterial;

			auto& overrideMaterial = ent.OverrideMaterial;

			const std::filesystem::path path = "assets/textures/jagged_slate_rock/";

			soso::TextureConfig config;
			config.Format = soso::ImageFormat::SRGBA;

			auto texture = soso::Texture2D::Create(config, path / "BaseColor.jpg");
			overrideMaterial->Set("u_BaseColorTexture", texture);
			
			config.Format = soso::ImageFormat::RGBA8;

			texture = soso::Texture2D::Create(config, path / "Normal.jpg");
			overrideMaterial->Set("u_NormalTexture", texture);
			overrideMaterial->Set("u_Material.HasNormalMap", true);
			
			texture = soso::Texture2D::Create(config, path / "Roughness.jpg");
			overrideMaterial->Set("u_RoughnessTexture", texture);

			overrideMaterial->Set("u_Material.BaseColor", glm::vec3(1.0f));
			overrideMaterial->Set("u_Material.Metallic", 0.0f);
			overrideMaterial->Set("u_Material.Roughness", 0.5f);
		}

		// Terrain
		{
			std::filesystem::path path = "assets/textures/jagged_slate_rock/Displacement.jpg";
			m_Terrain = std::make_unique<Terrain>(path, 100.0f, 100);

			Entity& ent = m_TerrainEnt;

			ent.Mesh = m_Terrain->GetTerrainMesh();
			ent.OverrideMaterial = m_DebugMaterial;
		}

		m_QTrans.Position = { 1, 1, 1 };

		SetEntityGrid(0,0, m_DefaultMesh);
	}

	void OnUpdate(soso::Timestep ts) override {

		m_FrameTime = ts;
		m_TotalTime += ts.GetSeconds();
		
		m_Camera.OnUpdate(ts);

		soso::Renderer::BeginScene(m_Camera);
		{
			
			for (auto&& ent : m_Entities) {
				
				if (ent.Render)
					soso::Renderer::SubmitMesh(ent.Mesh, ent.Transform.GetMatrix(), ent.OverrideMaterial);
			}
			
			soso::Renderer::SubmitQuad(m_DebugMaterial, m_QTrans.GetMatrix());
		}
		soso::Renderer::EndScene();
	}

	void OnEvent(soso::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void SetEntityGrid(uint32_t width, uint32_t height,
		const std::shared_ptr<soso::Mesh> mesh,
		const std::shared_ptr<soso::Material> overrideMaterial = nullptr,
		float spacing = 3.14f)
	{
		m_Entities.clear();
		m_Entities.reserve(width * height);

		m_Entities.emplace_back(m_Plane);
		m_Entities.emplace_back(m_TerrainEnt);

		for (uint32_t row = 0; row < height; row++) {

			for (uint32_t col = 0; col < width; col++) {

				Entity& ent = m_Entities.emplace_back();

				ent.Name = std::format("entity {},{}", col, row);

				ent.Mesh = mesh;
				ent.OverrideMaterial = overrideMaterial;
				ent.Transform.Position.x = col * spacing;
				ent.Transform.Position.z = row * spacing;
			}
		}
	}

	void OnImGuiRender() override {

		ImGui::Begin("Entity Editor");
		{
			// Entity Grid
			static int   uiWidth = 1;
			static int   uiHeight = 1;
			static float uiSpacing = 3.14f;

			ImGui::InputInt("Columns", &uiWidth);
			ImGui::InputInt("Rows", &uiHeight);
			uiWidth = glm::clamp(uiWidth, 1, 100);
			uiHeight = glm::clamp(uiHeight, 1, 100);

			ImGui::InputFloat("Spacing", &uiSpacing, 0.1f, 1.0f, "%.2f");
			uiSpacing = glm::max(uiSpacing, 0.01f);

			if (ImGui::Button("Generate Grid")) {

				SetEntityGrid(static_cast<uint32_t>(uiWidth), static_cast<uint32_t>(uiHeight), m_DefaultMesh, nullptr, uiSpacing);
			}


			// ------------------------ 
			static int selectedEntity = -1;
			static bool dirtyMaterial = false;

			std::string preview = (selectedEntity >= 0 && selectedEntity < m_Entities.size())
				? std::format("Entity {}", selectedEntity)
				: "Select Entity";

			if (ImGui::BeginCombo("Entities", preview.c_str())) {

				for (int i = 0; i < (int)m_Entities.size(); ++i) {

					bool is_selected = (selectedEntity == i);
					std::string label = std::format("Entity {}", i);

					if (ImGui::Selectable(label.c_str(), is_selected)) {

						selectedEntity = i;
						dirtyMaterial = false;
					}

					// ensure focus on the selected item
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();

			if (selectedEntity >= 0 && selectedEntity < m_Entities.size()) {

				Entity& e = m_Entities[selectedEntity];
				ImGui::Text("Editing %s", preview.c_str());

				ImGui::Separator();

				soso::UI::ToggleSwitch("Should Render", &e.Render);



				// Transform controls
				ImGui::DragFloat3("Position", &e.Transform.Position.x, 0.1f);
				ImGui::DragFloat3("Rotation", &e.Transform.Rotation.x, 1.0f, -360.0f, 360.0f, "%.1f°");
				if (ImGui::DragFloat("Scale", &e.Transform.Scale.x, 0.01f, 15.0f)) {

					e.Transform.Scale.y = e.Transform.Scale.x;
					e.Transform.Scale.z = e.Transform.Scale.x;
				}

				if (ImGui::Button("Reset Transform"))
					e.Transform = Transform();
				
				ImGui::NewLine();
				ImGui::Separator();
				ImGui::NewLine();

				static glm::vec3 uiBaseColor(1.0);
				static float uiMetalness = 0.5;
				static float uiRoughness = 0.5;

				if (ImGui::DragFloat3("Base Color", &uiBaseColor.x, 0.01f) || ImGui::DragFloat("Metalness", &uiMetalness, 0.01f) || ImGui::DragFloat("Roughness", &uiRoughness, 0.01f))
					dirtyMaterial = true;
				
				if (dirtyMaterial) {

					auto&& materials = e.Mesh->GetMaterials();

					for (auto&& mat : materials) {

						mat->Set("u_Material.BaseColor", uiBaseColor);
						mat->Set("u_Material.Metallic", uiMetalness);
						mat->Set("u_Material.Roughness", uiRoughness);
					}

					if (e.OverrideMaterial) {

						e.OverrideMaterial->Set("u_Material.BaseColor", uiBaseColor);
						e.OverrideMaterial->Set("u_Material.Metallic", uiMetalness);
						e.OverrideMaterial->Set("u_Material.Roughness", uiRoughness);
					}
				}

				ImGui::Separator();
			}
		}
		ImGui::End();

		ImGui::Begin("Graphics Settings");
		{
			bool vsync = soso::Application::Get().GetWindow().IsVSync();
			if (soso::UI::ToggleSwitch("Vsync", &vsync))
				soso::Application::Get().GetWindow().SetVSync(vsync);

			ImGui::Separator();
			static bool useNormalMap = true;
			if (soso::UI::ToggleSwitch("Normal Mapping", &useNormalMap)) {

				for (auto&& e : m_Entities) {

					if (e.OverrideMaterial)
						e.OverrideMaterial->Set("u_Material.HasNormalMap", useNormalMap);


					auto&& materials = e.Mesh->GetMaterials();
					for (auto& mat : materials) {
						mat->Set("u_Material.HasNormalMap", useNormalMap);
					}
				}
			}

			soso::UI::ToggleSwitch("Skybox", &m_RenderSkybox); // Skybox control

			ImGui::Separator();

		}
		ImGui::End();


		ImGui::Begin("Camera Settings");
		{
			bool isPerspective = m_Camera.GetCameraProjectionMode() == soso::CameraProjectionMode::PERSPECTIVE;

			ImGui::Text("Projection Mode");
			ImGui::SameLine();
			if (soso::UI::ToggleSwitch(" - ", &isPerspective)) {

				m_Camera.SetCameraProjectionMode(
					isPerspective
					? soso::CameraProjectionMode::PERSPECTIVE
					: soso::CameraProjectionMode::ORTHOGRAPHIC
				);
			}

			ImGui::SameLine();
			ImGui::TextUnformatted(
				isPerspective ? "Perspective" : "Orthographic"
			);


		}
		ImGui::End();
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
		ImGui::Begin("Statistics");
		{
			
			ImGui::Text("FPS: %.1f", 1 / m_FrameTime);
			ImGui::Text("Frame Time: %.1f ms", m_FrameTime.GetMilliseconds());

			ImGui::Separator();

			auto& renderStats = soso::Renderer::GetStats();
			ImGui::Text("Draw Calls: %d", renderStats.DrawCalls);
			ImGui::Text("Meshes: %d", renderStats.Meshes);
		}
		ImGui::End();
		ImGui::PopStyleVar();


		soso::Renderer::ImGuiRendererDebug();
	}


private:
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

	struct Entity {

		std::shared_ptr<soso::Mesh> Mesh;
		std::shared_ptr<soso::Material> OverrideMaterial = nullptr;
		Transform Transform{};
		
		std::string Name;
		bool Render = true;
	};

private:
	std::vector<Entity> m_Entities;
	Entity m_Plane;
	Entity m_TerrainEnt;

private:
	std::shared_ptr<soso::Material> m_DebugMaterial = nullptr;
	std::shared_ptr<soso::Mesh> m_DefaultMesh;
	std::shared_ptr<soso::TextureCube> m_TexCube;

	std::unique_ptr<Terrain> m_Terrain;

	Transform m_QTrans{};
	
	bool m_RenderSkybox = true;
	
	//---------------------------------------------
	soso::Timestep m_FrameTime;
	float m_TotalTime = 0.0f;
	std::shared_ptr<soso::ShaderLibrary> m_ShaderLibrary;
	soso::SceneCamera m_Camera;
};