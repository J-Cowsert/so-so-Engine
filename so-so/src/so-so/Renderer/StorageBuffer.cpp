#include "sspch.h"
#include "StorageBuffer.h"
#include "so-so/Renderer/Renderer.h"

#include "so-so/RenderAPI/OpenGL/OpenGLStorageBuffer.h"

namespace soso {

    std::shared_ptr<StorageBuffer> StorageBuffer::Create(uint32_t size, uint32_t bindingPoint) {

        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLStorageBuffer>(size, bindingPoint);
        }

        SS_CORE_ASSERT(false, "RendererAPI is undefined");
        return nullptr;
    }

}