#include "sspch.h"
#include "Input.h"
#include "so-so/Core/Application.h"
#include <GLFW/glfw3.h>

namespace soso {

	bool Input::IsKeyPressed(KeyCode keycode) {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode mousecode) {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int>(mousecode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	std::pair<float, float> Input::GetMousePosition() {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return { (float)xPos, (float)yPos };
	}

	float Input::GetMouseX() {

		auto [x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY() {

		auto [x, y] = GetMousePosition();
		return y;
	}
}