#include "sspch.h"
#include "RenderCommand.h"

#include "so-so/RendererAPI/OpenGL/OpenGLRendererAPI.h"

namespace soso {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}