#include "sspch.h"
#include "so-so/Core/Utils.h"
#include <GLFW/glfw3.h>


namespace soso {

	float Time::GetTime() {
		return glfwGetTime();
	}
}