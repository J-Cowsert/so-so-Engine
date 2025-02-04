#pragma once

#include "so-so/Core/Layer.h"

#include "so-so/Events/ApplicationEvent.h"
#include "so-so/Events/MouseEvent.h"
#include "so-so/Events/KeyEvent.h"

namespace soso {

	class SOSO_API ImGuiLayer : public Layer {
	
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

		void Begin();
		void End();
		
	private:
		float m_Time = 0.0f;
	};
}