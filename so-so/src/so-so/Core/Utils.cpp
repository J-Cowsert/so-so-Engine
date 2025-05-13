#include "sspch.h"

#include "Utils.h"
#include "GLFW/glfw3.h"

namespace soso {

	float Time::GetTime() {
		return glfwGetTime();
	}

}