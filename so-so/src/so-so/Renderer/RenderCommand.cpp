#include "sspch.h"
#include "RenderCommand.h"

#include "so-so/RenderAPI/OpenGL/OpenGLRendererAPI.h"

namespace soso {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}