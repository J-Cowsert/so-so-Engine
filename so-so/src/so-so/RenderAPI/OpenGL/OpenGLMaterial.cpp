#include "sspch.h"
#include "OpenGLMaterial.h"

#include <glad/glad.h>

#include "so-so/Core/Profiler.h"

// WIP

namespace soso {

    OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& targetUB, const std::string& name)
        : m_Shader(shader), m_Name(name), m_TargetUniformBufferName(targetUB)
    {
        AllocateByteBufferMemory();

        m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
        m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
    }

    OpenGLMaterial::OpenGLMaterial(std::shared_ptr<Material> material, const std::string& name)
        : m_Shader(material->GetShader()), m_Name(name)
    {
        AllocateByteBufferMemory();

        m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
        m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;

        auto glMat = std::static_pointer_cast<OpenGLMaterial>(material);
        m_UniformByteBuffer = ByteBuffer::Copy(glMat->m_UniformByteBuffer.Data, glMat->m_UniformByteBuffer.Size);
    }

    OpenGLMaterial::~OpenGLMaterial() {
        
        m_UniformByteBuffer.Release();
    }

    void OpenGLMaterial::Bind() {

        SS_PROFILE_FUNCTION();

        //m_Shader->Bind();

        if (m_IsDirty) {

            const auto& shaderBuffers = m_Shader->GetShaderBuffers();
            if (shaderBuffers.find(m_TargetUniformBufferName) != shaderBuffers.end()) {

                const auto& materialBuffer = shaderBuffers.at(m_TargetUniformBufferName);
                m_UniformBuffer = m_Shader->GetUniformBuffer(materialBuffer.BindingPoint);
            }

            m_IsDirty = false;
        }
            
        if (m_UniformBuffer) {

            m_UniformBuffer->SetData(m_UniformByteBuffer.Data, static_cast<uint32_t>(m_UniformByteBuffer.Size));
            m_UniformBuffer->Bind();
        }
       
        // TODO: Look into sampler objects
        for (auto& [binding, texture] : m_Texture2Ds) {
            
            if (texture) texture->Bind(binding);
        }

        for (auto& [binding, texture] : m_TextureCubes) {

            if (texture) texture->Bind(binding);
        }
    }

    void OpenGLMaterial::AllocateByteBufferMemory() {

        SS_PROFILE_FUNCTION();

        const auto& shaderBuffers = m_Shader->GetShaderBuffers();

        
        if (shaderBuffers.find(m_TargetUniformBufferName) == shaderBuffers.end()) {

            SS_CORE_WARN("Uniform-block '{0}' not found in shader '{1}'", m_TargetUniformBufferName, m_Shader->GetName());
            return;
        }


        const auto& materialBuffer = shaderBuffers.at(m_TargetUniformBufferName);

        SS_CORE_ASSERT(materialBuffer.Size > 0, "");

        m_UniformByteBuffer.Allocate(materialBuffer.Size);
        m_UniformByteBuffer.ZeroInitialize();
    }

    const ShaderUniform* OpenGLMaterial::FindUniformInfo(const std::string& name) {

        SS_PROFILE_FUNCTION();

        const auto& shaderBuffers = m_Shader->GetShaderBuffers();

        // --- Material must be the UniformBuffers name in the shader ---

        auto iter = shaderBuffers.find(m_TargetUniformBufferName);
        if (iter == shaderBuffers.end()) {
            return nullptr;
        }
      
        const ShaderUniformBufferInfo& materialBuffer = iter->second;

        if (materialBuffer.Uniforms.find(name) == materialBuffer.Uniforms.end()) {
            return nullptr;
        }

        return &materialBuffer.Uniforms.at(name);
    }

    const ShaderResourceInfo* OpenGLMaterial::FindResourceInfo(const std::string& name) {

        SS_PROFILE_FUNCTION();
        
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

    void OpenGLMaterial::Set(const std::string& name, const std::shared_ptr<Texture2D>& value) {

        const auto& info = FindResourceInfo(name);

        SS_CORE_ASSERT(info, "Could not find info");

        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return;
        }

        m_Texture2Ds[info->GetBindingPoint()] = value;
    }

    void OpenGLMaterial::Set(const std::string& name, const std::shared_ptr<TextureCube>& value) {

        const auto& info = FindResourceInfo(name);

        //SS_CORE_ASSERT(info, "Could not find info");

        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return;
        }

        m_TextureCubes[info->GetBindingPoint()] = value;
    }

    std::shared_ptr<TextureCube> OpenGLMaterial::GetTextureCube(const std::string& name) {

        const auto& info = FindResourceInfo(name);

        //SS_CORE_ASSERT(info, "Could not find info");

        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return nullptr;
        }

        auto& texCube = m_TextureCubes.at(info->GetBindingPoint());

        return texCube;
    }

}