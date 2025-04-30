#include "sspch.h"
#include "OpenGLMaterial.h"

#include <glad/glad.h>

namespace soso {

    OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& name) 
        : m_Shader(shader), m_Name(name)
    {
        
        AllocateByteBufferMemory();
    }

    OpenGLMaterial::~OpenGLMaterial() {
        
        m_UniformByteBuffer.Release();
    }

    void OpenGLMaterial::Bind() {

        m_Shader->Bind();
        
        if (m_IsDirty) {

            // Temporary solution
            const auto& shaderBuffers = m_Shader->GetShaderBuffers();
            if (shaderBuffers.find("Material") == shaderBuffers.end()) { SS_CORE_ERROR(""); return; }
            const auto& materialBuffer = shaderBuffers.at("Material");
            m_UniformBuffer = UniformBuffer::Create(m_UniformByteBuffer.Size, materialBuffer.BindingPoint);

            m_UniformBuffer->SetData(m_UniformByteBuffer.Data, m_UniformByteBuffer.Size);
            m_IsDirty = false;
        }

        for (auto& [binding, texture] : m_Textures) {

            if (texture) {

                // TODO: sampler objects
                texture->Bind(binding);
                
            }
        }

    }

    void OpenGLMaterial::AllocateByteBufferMemory() {

        const auto& shaderBuffers = m_Shader->GetShaderBuffers();

        // --- Material must be the UniformBuffers name in the shader ---
        if (shaderBuffers.find("Material") == shaderBuffers.end()) {
            return;
        }
        const auto& materialBuffer = shaderBuffers.at("Material");

        SS_CORE_ASSERT(materialBuffer.Size > 0, "");

        m_UniformByteBuffer.Allocate(materialBuffer.Size);
        m_UniformByteBuffer.ZeroInitialize();
    }

    const ShaderUniform* OpenGLMaterial::FindUniformInfo(const std::string& name) {

        const auto& shaderBuffers = m_Shader->GetShaderBuffers();

        // --- Material must be the UniformBuffers name in the shader ---
        if (shaderBuffers.find("Material") == shaderBuffers.end()) {
            return nullptr;
        }
        const auto& materialBuffer = shaderBuffers.at("Material");

        if (materialBuffer.Uniforms.find(name) == materialBuffer.Uniforms.end()) {
            return nullptr;
        }

        return &materialBuffer.Uniforms.at(name);
    }

    const ShaderResourceInfo* OpenGLMaterial::FindResourceInfo(const std::string& name) {
        
        auto& resources = m_Shader->GetResources();
        for (const auto& [n, resource] : resources) {
            if (resource.GetName() == name)
                return &resource;
        }
        return nullptr;
    }

    void OpenGLMaterial::Set(const std::string& name, const float value) {

        Set<float>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const int value) {

        Set<int>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec2 value) {
        
        Set<glm::vec2>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec3 value) {
        
        Set<glm::vec3>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::vec4 value) {
        
        Set<glm::vec4>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::mat2 value) {
        
        Set<glm::mat2>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::mat3 value) {
    
        Set<glm::mat3>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const glm::mat4 value) {
    
        Set<glm::mat4>(name, value);
    }

    void OpenGLMaterial::Set(const std::string& name, const std::shared_ptr<Texture2D>& texture) {

        const auto& info = FindResourceInfo(name);
        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return;
        }

        m_Textures[info->GetBindingPoint()] = texture;
    }

}