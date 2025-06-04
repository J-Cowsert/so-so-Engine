#pragma once

#include "so-so/Core/Layer.h"

#include "so-so/Events/ApplicationEvent.h"
#include "so-so/Events/MouseEvent.h"
#include "so-so/Events/KeyEvent.h"

namespace soso {

	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;

		void Begin();
		void End();
	};
}