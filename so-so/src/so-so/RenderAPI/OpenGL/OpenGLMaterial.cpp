#include "sspch.h"
#include "OpenGLMaterial.h"

#include <glad/glad.h>

// WIP

namespace soso {

    OpenGLMaterial::OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& name) 
        : m_Shader(shader), m_Name(name)
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

        //m_Shader->Bind();

        if (m_IsDirty) {

            const auto& shaderBuffers = m_Shader->GetShaderBuffers();
            if (shaderBuffers.find("Material") != shaderBuffers.end()) {

                const auto& materialBuffer = shaderBuffers.at("Material");
                m_UniformBuffer = m_Shader->GetUniformBuffer(materialBuffer.BindingPoint);
            }

            m_IsDirty = false;
        }
            
        if (m_UniformBuffer) {

            m_UniformBuffer->SetData(m_UniformByteBuffer.Data, static_cast<uint32_t>(m_UniformByteBuffer.Size));
            m_UniformBuffer->Bind();
        }
       
        for (auto&& [binding, texture] : m_Texture2Ds) {
            
            // TODO: Look into sampler objects
            if (texture) texture->Bind(binding);
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

        auto iter = shaderBuffers.find("Material");
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

        //SS_CORE_ASSERT(info, "Could not find info");

        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return;
        }

        m_Texture2Ds[info->GetBindingPoint()] = value;
    }

    void OpenGLMaterial::Set(const std::string& name, const std::shared_ptr<TextureCube>& value) {

        const auto& info = FindResourceInfo(name);

        SS_CORE_ASSERT(info, "Could not find info");

        if (!info) {
            SS_CORE_WARN("Could not find resource: {0}", name);
            return;
        }

        m_Texture2Ds[info->GetBindingPoint()] = value;
    }

    std::shared_ptr<TextureCube> OpenGLMaterial::GetTextureCube(const std::string& name) {
        SS_CORE_ASSERT(false, "not implemented");

        const auto& info = FindResourceInfo(name);
        SS_CORE_ASSERT(info, "Could not find info");
        auto& texCube = m_Texture2Ds.at(info->GetBindingPoint());
        return nullptr;
    }

}