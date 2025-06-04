#pragma once

#include "Misc/ShaderEditor.h"

#include <soso.h>

// Temporary Editor

class EditorLayer : public soso::Layer {
public:

	EditorLayer() = default;
	EditorLayer(std::unique_ptr<soso::Layer> runtime)
		:Layer("Editor"), m_RuntimeLayer(std::move(runtime))
	{
	}

	void OnAttach() override {
		
		m_FrameBuffer = soso::Renderer::GetCompositeFrameBuffer();

		m_RuntimeLayer->OnAttach();
	}

	void OnUpdate(soso::Timestep ts) override {
		
		const auto& config = m_FrameBuffer->GetConfig();
		if ( m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (config.Width != m_ViewportSize.x || config.Height != m_ViewportSize.y)) {

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		{
			m_RuntimeLayer->OnUpdate(ts);
		}
	}

	void OnEvent(soso::Event& event) override {
		m_RuntimeLayer->OnEvent(event);
	}

	bool OnWindowResize(soso::WindowResizeEvent& event) {
		
		return true;
	}

	void OnImGuiRender() override {
		
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) 
		{
			if (ImGui::BeginMenu("Window"))
			{

				if (ImGui::MenuItem("Fullscreen")) 
				{
					soso::Window& window = soso::Application::Get().GetWindow();
					window.SetFullscreen(window.IsFullscreen() ? false : true);
				}

				if (ImGui::MenuItem("Exit")) soso::Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools")) {

				if (ImGui::MenuItem("Metrics Tool")) {

					m_ShowMetricsTool = !m_ShowMetricsTool;
				}

				if (ImGui::MenuItem("Stack Tool")) {

					m_ShowStackTool = !m_ShowStackTool;
				}

				if (ImGui::MenuItem("ShaderEditor")) {

					m_ShowShaderEditorTool = !m_ShowShaderEditorTool;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		if (m_ShowMetricsTool)
			ImGui::ShowMetricsWindow(&m_ShowMetricsTool);
		
		if (m_ShowStackTool)
			ImGui::ShowStackToolWindow(&m_ShowStackTool);

		if (m_ShowShaderEditorTool)
			m_ShaderEditor.Draw();

		ImGui::Begin("Viewport");
		{
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize)) 
			{
				m_FrameBuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			}

			uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
			ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}


		{
			m_RuntimeLayer->OnImGuiRender();
		}


		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

private:

	std::shared_ptr<soso::FrameBuffer> m_FrameBuffer;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	//glm::vec2 m_ViewportBounds[2];

private:
	std::unique_ptr<soso::Layer> m_RuntimeLayer;

	ShaderEditor m_ShaderEditor;

private:
	bool m_ShowMetricsTool = false;
	bool m_ShowStackTool = false;
	bool m_ShowShaderEditorTool = true;
};