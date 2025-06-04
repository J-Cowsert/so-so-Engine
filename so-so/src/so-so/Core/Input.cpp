#include "sspch.h"
#include "Input.h"

#include "so-so/Core/Application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace soso {

	bool Input::IsKeyPressed(KeyCode keycode) {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;

		//auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//ImGuiContext* context = ImGui::GetCurrentContext();
		//bool pressed = false;
		//
		//for (ImGuiViewport* viewport : context->Viewports) {
		//
		//	if (!viewport->PlatformUserData) continue;
		//
		//	GLFWwindow* windowHandle = *(GLFWwindow**)viewport->PlatformUserData;
		//
		//	if (!windowHandle) continue;
		//
		//	auto state = glfwGetKey(windowHandle, static_cast<int>(keycode));
		//
		//	if (state == GLFW_PRESS || state == GLFW_REPEAT) {
		//
		//		pressed = true;
		//		break;
		//	}
		//}
		//
		//return pressed;
	}

	bool Input::IsMouseButtonPressed(MouseCode mousecode) {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int>(mousecode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;

		//auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//ImGuiContext* context = ImGui::GetCurrentContext();
		//bool pressed = false;
		//
		//for (ImGuiViewport* viewport : context->Viewports) {
		//	
		//	if (!viewport->PlatformUserData) continue;
		//
		//	GLFWwindow* windowHandle = *(GLFWwindow**)viewport->PlatformUserData;
		//
		//	if (!windowHandle) continue;
		//
		//	auto state = glfwGetMouseButton(windowHandle, static_cast<int>(mousecode));
		//
		//	if (state == GLFW_PRESS || state == GLFW_REPEAT) {
		//		
		//		pressed = true;
		//		break;
		//	}
		//}
		//
		//return pressed;
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

	void Input::SetCursorMode(CursorMode mode) {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}

	CursorMode Input::GetCursorMode() {

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		return (CursorMode)(glfwGetInputMode(window, GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
	}
}