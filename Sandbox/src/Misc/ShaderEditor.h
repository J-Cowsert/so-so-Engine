#pragma once

#include "so-so/Renderer/Shader.h"

#include <vector>
#include "imgui.h"

class ShaderEditor {
public:
	ShaderEditor();

	void Draw();

private:
	std::vector<std::shared_ptr<soso::Shader>> m_Shaders;
	
	struct ShaderInfo {

		std::shared_ptr<soso::Shader> Shader = nullptr;
		std::string SourceBuffer;
	};

	ShaderInfo m_Selected;
};