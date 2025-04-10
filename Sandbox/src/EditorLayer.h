#pragma once


#pragma once

#include <soso.h>
#include "so-so/Core/Utils.h"
#include "so-so/Renderer/FrameBuffer.h"
#include "so-so/Renderer/Renderer.h"

#include "glm/ext.hpp"
#include "imgui.h"

// Temporary Editor

class EditorLayer : public soso::Layer {
public:

	EditorLayer()
		:Layer("Editor") {

		
	}

	void OnAttach() override {

		soso::FrameBufferConfig fbConfig;
		fbConfig.Attachments = { soso::FrameBufferTextureFormat::RGBA8, soso::FrameBufferTextureFormat::RED_INTEGER, soso::FrameBufferTextureFormat::DEPTH24STENCIL8 };
		fbConfig.Width = 1280;
		fbConfig.Height = 720;
		
		m_FrameBuffer = soso::FrameBuffer::Create(fbConfig);


		m_RuntimeLayer->OnAttach();
	}

	void OnUpdate(soso::Timestep ts) override {

		if (soso::FrameBufferConfig config = m_FrameBuffer->GetConfig(); m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(config.Width != m_ViewportSize.x || config.Height != m_ViewportSize.y)) {

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_FrameBuffer->Bind();
		soso::RenderCommand::SetClearColor({ 1.0f, 0.1f, 0.1f, 1 });
		soso::RenderCommand::Clear();

		float time = soso::Time::GetTime();
		soso::Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

	
		{
			m_RuntimeLayer->OnUpdate(ts);
		}


		// Clear our entity ID attachment to -1
		m_FrameBuffer->ClearAttachment(1, -1);
		m_FrameBuffer->Unbind();
	}

	void OnEvent(soso::Event& event) override {
		m_RuntimeLayer->OnEvent(event);
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


		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File"))
			{
				//if (ImGui::MenuItem("Exit")) soso::Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });


		ImGui::Begin("Viewport");
		{
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize)) 
			{
				m_FrameBuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };



				//m_RuntimeLayer->SetResolution(m_ViewportSize);



			}
			uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}


		{

			m_RuntimeLayer->OnImGuiRender();
		}


		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SetRuntimeLayer(std::unique_ptr<soso::Layer> layer) {
		m_RuntimeLayer = std::move(layer);
	}

private:

	std::shared_ptr<soso::FrameBuffer> m_FrameBuffer;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	//glm::vec2 m_ViewportBounds[2];

	double m_LastFrameTime = 0;



private:
	std::unique_ptr<soso::Layer> m_RuntimeLayer;
};