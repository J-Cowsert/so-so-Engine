#pragma once

#include "Core.h"
#include "Timestep.h"
#include "so-so/Events/Event.h"

namespace soso {

	class Layer {

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		const std::string& m_DebugName;
	};
}