
// https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide
#include "sspch.h"
#include "OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <format>

#include <shaderc/shaderc.hpp>
#include <file_finder.h>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

#define SHADER_DEBUG_LOG false // Toggle for debug logging

#if SHADER_DEBUG_LOG
	#define SHADER_DEBUG(...) SS_CORE_TRACE(__VA_ARGS__)
#else
	#define SHADER_DEBUG(...)
#endif

namespace soso {

	namespace Utils {

		//TODO: Clean up this mess

		static GLenum StringToGLShaderStage(const std::string& type) {

			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			SS_CORE_ASSERT(false, "Unknown shader type");
			return 0;
		} 

		static ShaderStage StringToSosoShaderStage(const std::string& type) {

			if (type == "vertex")
				return ShaderStage::Vertex;
			if (type == "fragment" || type == "pixel")
				return ShaderStage::Fragment;

			SS_CORE_ASSERT(false, "Unknown shader type");
			return ShaderStage::None;
		}

		static GLenum SosoShaderStageToGLShaderStage(const ShaderStage type) {

			switch (type) {
				case ShaderStage::Vertex:   return GL_VERTEX_SHADER;
				case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
			}

			SS_CORE_ASSERT(false, "Unknown shader type");
			return 0;
		}

		static const char* SosoShaderStageToString(const ShaderStage type) {

			switch (type) {
			case ShaderStage::Vertex:   return "Vertex";
			case ShaderStage::Fragment: return "Fragment";
			}

			SS_CORE_ASSERT(false, "Unknown shader type");
			return nullptr;
		}

		static ShaderStage GLShaderStageToSosoShaderStage(GLenum type) {

			switch (type) {
				case GL_VERTEX_SHADER:   return ShaderStage::Vertex;
				case GL_FRAGMENT_SHADER: return ShaderStage::Fragment;
			}
			SS_CORE_ASSERT(false, "");
			return ShaderStage::None;
		}

		static const char* GLShaderStageToString(GLenum stage) {

			switch (stage) {
				case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			SS_CORE_ASSERT(false, "");
			return nullptr;
		}

		static const char* GLTypeToString(GLenum type) {
			switch (type) {
			case GL_FLOAT: return "float";
			case GL_FLOAT_VEC2: return "vec2";
			case GL_FLOAT_VEC3: return "vec3";
			case GL_FLOAT_VEC4: return "vec4";
			case GL_INT: return "int";
			case GL_UNSIGNED_INT: return "uint";
			case GL_BOOL: return "bool";
			case GL_FLOAT_MAT2: return "mat2";
			case GL_FLOAT_MAT3: return "mat3";
			case GL_FLOAT_MAT4: return "mat4";
			case GL_SAMPLER_2D: return "sampler2D";
			case GL_SAMPLER_CUBE: return "samplerCube";
			}
			SS_CORE_ASSERT(false, "");
			return nullptr;
		}

		static uint32_t GLTypeSize(GLenum type) {
			switch (type) {
			case GL_BOOL:
			case GL_INT:
			case GL_FLOAT:      return 4;
			case GL_FLOAT_VEC2: return 8;
			case GL_FLOAT_VEC3: return 12;
			case GL_FLOAT_VEC4: return 16;
			case GL_FLOAT_MAT3: return 48; // std140 3 × vec4 (16‑byte rows)
			case GL_FLOAT_MAT4: return 64; // 4 × vec4
			}
			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static ShaderUniformType GLTypeToShaderUniformType(GLenum type) {
			switch (type) {
			case GL_BOOL:           return ShaderUniformType::Bool;
			case GL_INT:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:   return ShaderUniformType::Int;   // samplers are set with glUniform1i
			case GL_FLOAT:          return ShaderUniformType::Float;
			case GL_FLOAT_VEC2:     return ShaderUniformType::Vec2;
			case GL_FLOAT_VEC3:     return ShaderUniformType::Vec3;
			case GL_FLOAT_VEC4:     return ShaderUniformType::Vec4;
			case GL_FLOAT_MAT3:     return ShaderUniformType::Mat3;
			case GL_FLOAT_MAT4:     return ShaderUniformType::Mat4;
			}
			SS_CORE_ASSERT(false, "");
			return ShaderUniformType::None;
		}

		static const char* ShaderUniformTypeToString(ShaderUniformType type) {
			switch (type) {
			case ShaderUniformType::None:  return "None";
			case ShaderUniformType::Bool:  return "Bool";
			case ShaderUniformType::Int:   return "Int";
			case ShaderUniformType::Float: return "Float";
			case ShaderUniformType::Vec2:  return "Vec2";
			case ShaderUniformType::Vec3:  return "Vec3";
			case ShaderUniformType::Vec4:  return "Vec4";
			case ShaderUniformType::Mat3:  return "Mat3";
			case ShaderUniformType::Mat4:  return "Mat4";
			}
			SS_CORE_ASSERT(false, "");
			return nullptr;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage) {
			switch (stage) {
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			SS_CORE_ASSERT(false, "");
			return (shaderc_shader_kind)0;
		}


		static const char* GetCacheDirectory() {
			
			return "Resources/Cache/Shader/OpenGL";
		}

		static const char* GLShaderStageCachedFileExtension(ShaderStage source) {
			switch (source) {
			case ShaderStage::Vertex:    return ".cached.vert";
			case ShaderStage::Fragment:  return ".cached.frag";
			}
			SS_CORE_ASSERT(false, "");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(ShaderStage stage) {
			switch (stage) {
			case ShaderStage::Vertex:    return ".cached_vulkan.vert";
			case ShaderStage::Fragment:  return ".cached_vulkan.frag";
			}
			SS_CORE_ASSERT(false, "");
			return "";
		}

		static void CreateCacheDirectoryIfNeeded() {
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static ShaderUniformType SPIRTypeToShaderUniformType(spirv_cross::SPIRType type) {
			switch (type.basetype) {
			case spirv_cross::SPIRType::Boolean:  return ShaderUniformType::Bool;
			case spirv_cross::SPIRType::Int:
				if (type.vecsize == 1)            return ShaderUniformType::Int;
				if (type.vecsize == 2)            return ShaderUniformType::IVec2;
				if (type.vecsize == 3)            return ShaderUniformType::IVec3;
				if (type.vecsize == 4)            return ShaderUniformType::IVec4;

			case spirv_cross::SPIRType::UInt:     return ShaderUniformType::UInt;
			case spirv_cross::SPIRType::Float:
				if (type.columns == 3)            return ShaderUniformType::Mat3;
				if (type.columns == 4)            return ShaderUniformType::Mat4;

				if (type.vecsize == 1)            return ShaderUniformType::Float;
				if (type.vecsize == 2)            return ShaderUniformType::Vec2;
				if (type.vecsize == 3)            return ShaderUniformType::Vec3;
				if (type.vecsize == 4)            return ShaderUniformType::Vec4;
				break;
			}
			SS_CORE_ASSERT(false, "");
			return ShaderUniformType::None;
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) 
		: m_Filepath(filepath) {

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto sources = PreProcess(source);
		Compile(sources);


		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		m_Name = filepath.substr(lastSlash, count);
	
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		: m_Name(name) {

		Utils::CreateCacheDirectoryIfNeeded();

		std::unordered_map<ShaderStage, std::string> sources;
		sources[ShaderStage::Vertex] = vertexSource;
		sources[ShaderStage::Fragment] = fragmentSource;

		Compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Reload() const {

	}

	std::string OpenGLShader::ReadFile(const std::string& filepath) {

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1) {
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else {
				SS_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else {
			SS_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<ShaderStage, std::string> OpenGLShader::PreProcess(const std::string& source) {

		std::unordered_map<ShaderStage, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line

		while (pos != std::string::npos) {

			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			SS_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			SS_CORE_ASSERT(Utils::StringToGLShaderStage(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			SS_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::StringToSosoShaderStage(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(std::unordered_map<ShaderStage, std::string> shaderSources, bool forceReload) {
		
		SS_CORE_WARN("Compiling shader - {0}", m_Filepath);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		options.SetGenerateDebugInfo();

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_OpenGLSpirv;
		shaderData.clear();
		
		for (auto&& [stage, source] : shaderSources) {

			std::filesystem::path shaderFilePath = m_Filepath;
			std::filesystem::path cachePath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedFileExtension(stage));

			std::ifstream in(cachePath, std::ios::in | std::ios::binary);
			if (in.is_open() && !forceReload) {

				// Read cached file and store in buffer.
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else {
				
				// Compile and store in buffer
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(Utils::SosoShaderStageToGLShaderStage(stage)), m_Filepath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
					SS_CORE_ERROR(module.GetErrorMessage());
					SS_CORE_ASSERT(false, Utils::SosoShaderStageToString(stage));
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				// Write binary to cache file
				std::ofstream out(cachePath, std::ios::out | std::ios::binary);
				if (out.is_open()) {
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		// Reflect Shader Info
		for (auto&& [stage, data] : shaderData) {
			Reflect(stage, data);
		}


		// Upload shaders to GPU
		GLuint program = glCreateProgram();

		// Create shaders from spirv binary and link them into program
		std::vector<GLenum> shaderIDs;
		for (auto&& [stage, data] : shaderData) {

			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(Utils::SosoShaderStageToGLShaderStage(stage)));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, data.data(), data.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);
		
		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

		if (isLinked == false) {

			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			SS_CORE_ERROR("Shader linking failed {0}:\n\t{1}", m_Filepath, infoLog.data());

			glDeleteProgram(program);
			for (auto ID : shaderIDs) {
				glDeleteShader(ID);
			}

			SS_CORE_ASSERT(false, "Shader link failed.");
			return;
		}

		for (auto ID : shaderIDs) {
			glDetachShader(program, ID);
			glDeleteShader(ID);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Upload() {

		// TODO
	}

	void OpenGLShader::Reflect(ShaderStage stage, const std::vector<uint32_t>& shaderData) {

		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		SS_CORE_WARN("OpenGLShader::Reflect - {0} - {1}", Utils::SosoShaderStageToString(stage), m_Filepath);
		SHADER_DEBUG("    {0} uniform buffers", resources.uniform_buffers.size());
		SHADER_DEBUG("    {0} resources", resources.sampled_images.size());

		// ---- Uniform Buffer Objects ----
		SHADER_DEBUG("Uniform buffer objects:");
		for (const auto& resource : resources.uniform_buffers) {
			
			auto activeBuffers = compiler.get_active_buffer_ranges(resource.id);
			if (activeBuffers.size()) {

				std::string blockName = resource.name;
				std::string instanceName = compiler.get_name(resource.id);
				const auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t    binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				uint32_t    index = compiler.get_decoration(resource.id, spv::DecorationIndex);
				uint32_t    bufferSize = (uint32_t)compiler.get_declared_struct_size(bufferType);
				int         memberCount = (uint32_t)bufferType.member_types.size();

				SHADER_DEBUG("  {0}", blockName);
				SHADER_DEBUG("    Size = {0}", bufferSize);
				SHADER_DEBUG("    Binding = {0}", binding);
				SHADER_DEBUG("    Members = {0}", memberCount);

				// Create ShaderBuffer and pass into reflection data. 
				ShaderBuffer& buffer = m_ReflectionData.ShaderBuffers[blockName];
				buffer.Name = blockName;
				buffer.Size = bufferSize;
				buffer.BindingPoint = binding;

				std::unordered_map<std::string, ShaderUniform>& bufferUniforms = buffer.Uniforms;
				for (uint32_t i = 0; i < memberCount; ++i) {

					// ----- per‑member reflection -----
					std::string  memberName = compiler.get_member_name(bufferType.self, i);
					const auto&  memberType = compiler.get_type(bufferType.member_types[i]);
					uint32_t     offset = compiler.type_struct_member_offset(bufferType, i);
					uint32_t     size = compiler.get_declared_struct_member_size(bufferType, i);

					SHADER_DEBUG("    [{0}] {1}", i, memberName);
					SHADER_DEBUG("         offset = {0}", offset);
					SHADER_DEBUG("         size   = {0}", size);

					// Combine Uniform's name with its parent UniformBuffer's name - i.e. "u_Material.Shininess"  
					std::string uniformName = std::format("{}.{}", instanceName, memberName);
					bufferUniforms[uniformName] = ShaderUniform{ uniformName, Utils::SPIRTypeToShaderUniformType(memberType), size, offset };
				}
			}

			//SS_CORE_WARN("m_ShaderBuffers.size() - {0}", m_ReflectionData.ShaderBuffers.size());
		}

		// ---- Resources ----
		for (auto& resource : resources.sampled_images) {

			const auto& name = resource.name;
			auto& baseType = compiler.get_type(resource.base_type_id);
			auto& type = compiler.get_type(resource.type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = baseType.image.dim;
			uint32_t arraySize = type.array.size() > 0 ? type.array[0] : 1;

			SHADER_DEBUG("Sampler `{}` binding {}", name, binding);

			m_ReflectionData.Resources[name] = ShaderResourceInfo{ name, binding };
		}


		// Debug Print out all reflection data from containers		
		SHADER_DEBUG("------Reflection ShaderBuffers debug info------");
		for (const auto& [name, shaderBuffer] : m_ReflectionData.ShaderBuffers) {

			SHADER_DEBUG("Uniform Buffer - Name: {0}, Size: {1}, Binding: {2}", shaderBuffer.Name, shaderBuffer.Size, shaderBuffer.BindingPoint);

			for (const auto& [bufferName, uniform] : shaderBuffer.Uniforms) {
				SHADER_DEBUG("Uniform - Name: {0}, Type: {1}, Size: {2}, Offset: {3}",
					uniform.GetName(),
					Utils::ShaderUniformTypeToString(uniform.GetType()),
					uniform.GetSize(),
					uniform.GetOffset()
				);
			}
		}

		for (const auto& [name, resourceInfo] : m_ReflectionData.Resources) {

			SHADER_DEBUG("Resource - Name: {0}, BindingPoint: {1}", resourceInfo.GetName(), resourceInfo.GetBindingPoint());
		}
		SHADER_DEBUG("--------------------------------------");
	}

	void OpenGLShader::Bind() const {
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::SetUniform(const std::string& name, float value) {
		UploadFloat(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, int value) {
		UploadInt(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec2& value) {
		UploadVec2(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec3& value) {
		UploadVec3(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec4& value) {
		UploadVec4(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::mat3& matrix) {
		UploadMat3(name, matrix);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::mat4& matrix) {
		UploadMat4(name, matrix);
	}

	void OpenGLShader::UploadFloat(const std::string& name, float value) {
		
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadInt(const std::string& name, int value) {

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadVec2(const std::string& name, const glm::vec2& value) {
		
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadVec3(const std::string& name, const glm::vec3& value) {
		
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadVec4(const std::string& name, const glm::vec4& value) {

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadMat3(const std::string& name, const glm::mat3& matrix) {

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadMat4(const std::string& name, const glm::mat4& matrix) {
		
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}