#include "sspch.h"
#include "so-so/RenderAPI/OpenGL/OpenGLTexture.h"

#include "so-so/Resource/TextureImporter.h"

namespace soso {

	namespace Utils {

		static GLenum SosoImageFormatToGLDataFormat(ImageFormat format) {

			switch (format) {
				case ImageFormat::R8:      return GL_RED;
				case ImageFormat::RGB8:    return GL_RGB;
				case ImageFormat::RGBA8:   return GL_RGBA;
				case ImageFormat::RGBA16F: return GL_RGBA;
				case ImageFormat::RGBA32F: return GL_RGBA;

				case ImageFormat::SRGB:    return GL_RGB; 
				case ImageFormat::SRGBA:   return GL_RGBA;

			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum SosoImageFormatToGLInternalFormat(ImageFormat format) {

			switch (format) {
				case ImageFormat::R8:      return GL_R8;
				case ImageFormat::RGB8:    return GL_RGB8;
				case ImageFormat::RGBA8:   return GL_RGBA8;
				case ImageFormat::RGBA16F: return GL_RGBA16F;
				case ImageFormat::RGBA32F: return GL_RGBA32F;

				case ImageFormat::SRGB:    return GL_SRGB8;
				case ImageFormat::SRGBA:   return GL_SRGB8_ALPHA8;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum GLSamplerFilter(SamplerFilter filter) {

			switch (filter) {
				case SamplerFilter::Linear: return GL_LINEAR;
				case SamplerFilter::Nearest: return GL_NEAREST;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum GLSamplerWrap(SamplerWrap wrap) {

			switch (wrap) {
				case SamplerWrap::Repeat: return GL_REPEAT;
				case SamplerWrap::Clamp: return GL_CLAMP_TO_EDGE;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static size_t GetMemorySize(ImageFormat format, uint32_t width, uint32_t height)
		{

			size_t bpp = 0;
			switch (format)
			{
			case ImageFormat::R8:      bpp = 1;                    break; //  1 ×  8-bit
			case ImageFormat::RGB8:    bpp = 3;                    break; //  3 ×  8-bit
			case ImageFormat::RGBA8:   bpp = 4;                    break; //  4 ×  8-bit
			case ImageFormat::SRGB:    bpp = 3;                    break; //  3 ×  8-bit
			case ImageFormat::SRGBA:   bpp = 4;                    break; //  4 ×  8-bit
			case ImageFormat::RGBA16F: bpp = 4 * sizeof(uint16_t); break; //  4 × 16-bit float
			case ImageFormat::RGBA32F: bpp = 4 * sizeof(float);    break; //  4 × 32-bit float
			default:
				SS_CORE_ASSERT(false, "Unknown ImageFormat");
				return 0;
			}
			return width * height * bpp;
		}

		static uint32_t CalculateMipMapCount(uint32_t width, uint32_t height) {
			uint32_t levels = 1;
			while ((width | height) >> levels)
				levels++;

			return levels;
		}

	}

	
	OpenGLTexture2D::OpenGLTexture2D(const TextureConfig& config, const std::filesystem::path& filepath) 
		: m_Config(config), m_Filepath(filepath)
	{
		// Initialize unique resource ID
		ID = {};

		ImageResult result;
		result = TextureImporter::LoadImageFromFile(filepath);
		if (!result.Buffer.Data) {

			result = TextureImporter::LoadImageFromFile("Resources/Texture/error_texture_256.png");
			SS_CORE_ASSERT(result.Buffer.Data, "");
		}

		m_ImageData = result.Buffer;
		m_Config.Format = (config.Format == ImageFormat::SRGB || config.Format == ImageFormat::SRGBA) ? config.Format : result.Format; // Might break if image is HDR but config.format is passed in as SRGB
		m_Config.Width = result.Width;
		m_Config.Height = result.Height;


		m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(m_Config.Format);
		m_DataFormat = Utils::SosoImageFormatToGLDataFormat(m_Config.Format);

		// Decide mip levels
		uint32_t levels = m_Config.GenerateMips ? Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height) : 1;

		// Create and allocate
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureHandle);
		glTextureStorage2D(m_TextureHandle, levels, m_InternalFormat, m_Config.Width, m_Config.Height);

		GLenum filter = Utils::GLSamplerFilter(m_Config.Filter);
		GLenum wrap = Utils::GLSamplerWrap(m_Config.Wrap);

		// Set filtering
		if (m_Config.GenerateMips) {
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, filter);
		}
		else {
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, filter);
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, filter);
		}

		// Set wrap
		glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_T, wrap);

		// Enable max quality anisotropic filtering
		float maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
		glTextureParameterf(m_TextureHandle, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);


		GLenum type = (m_Config.Format == ImageFormat::RGBA32F || m_Config.Format == ImageFormat::RGBA16F) ? GL_FLOAT : GL_UNSIGNED_BYTE;

		// Upload level 0 data
		glTextureSubImage2D(
			m_TextureHandle, 0, 0, 0,
			m_Config.Width, m_Config.Height,
			m_DataFormat, type,
			m_ImageData.Data
		);

		// Generate mips
		if (m_Config.GenerateMips && levels > 1) {
			glGenerateTextureMipmap(m_TextureHandle);
		}
		
		m_IsLoaded = true;
		m_ImageData.Release();
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureConfig& config, ByteBuffer data)
		: m_Config(config)
	{
		// Initialize unique resource ID
		ID = {};

		if (config.Height == 0) {

			ImageResult result;
			result = TextureImporter::LoadImageFromBuffer(data);
			if (!result.Buffer.Data) {

				result = TextureImporter::LoadImageFromFile("Resources/Texture/error_texture_256.png");
				SS_CORE_ASSERT(result.Buffer.Data, "");
			}

			m_ImageData = result.Buffer;
			m_Config.Format = (config.Format == ImageFormat::SRGB || config.Format == ImageFormat::SRGBA) ? config.Format : result.Format; // Breaks if image result format is HDR but config.format is passed in as SRGB
			m_Config.Width = result.Width;
			m_Config.Height = result.Height;
		}
		else if (data) {

			m_ImageData = ByteBuffer::Copy(data.Data, data.Size); // If imageData is stored in the future size needs to be recalculated based on the image format.
		}
		else {

			auto size = (uint32_t)Utils::GetMemorySize(m_Config.Format, m_Config.Width, m_Config.Height);
			m_ImageData.Allocate(size);
			m_ImageData.ZeroInitialize();
		}

		m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(m_Config.Format);
		m_DataFormat = Utils::SosoImageFormatToGLDataFormat(m_Config.Format);

		// Decide mip levels
		uint32_t levels = m_Config.GenerateMips ? Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height) : 1;

		// Create and allocate
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureHandle);
		glTextureStorage2D(m_TextureHandle, levels, m_InternalFormat, m_Config.Width, m_Config.Height);

		GLenum filter = Utils::GLSamplerFilter(m_Config.Filter);
		GLenum wrap = Utils::GLSamplerWrap(m_Config.Wrap); // SamplerWrap::Clamp -> GL_CLAMP_TO_EDGE;

		// Set filtering
		if (m_Config.GenerateMips) {
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, filter);
		}
		else {
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, filter);
			glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, filter);
		}

		// Set wrap
		glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_T, wrap);

		// Enable max quality anisotropic filtering
		float maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
		glTextureParameterf(m_TextureHandle, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

		GLenum type = (m_Config.Format == ImageFormat::RGBA32F || m_Config.Format == ImageFormat::RGBA16F) ? GL_FLOAT : GL_UNSIGNED_BYTE;

		// Upload level 0 data
		glTextureSubImage2D(
			m_TextureHandle, 0, 0, 0,
			m_Config.Width, m_Config.Height,
			m_DataFormat, type,
			m_ImageData.Data
		);

		// Generate mips
		if (m_Config.GenerateMips && levels > 1) {
			glGenerateTextureMipmap(m_TextureHandle);
		}

		m_IsLoaded = true;
		m_ImageData.Release();
	}


	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_TextureHandle);

		if (m_ImageData.Data)
			m_ImageData.Release();
	}

	uint32_t OpenGLTexture2D::GetNumMipLevels() const {

		return Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height);
	}

	void OpenGLTexture2D::SetData(ByteBuffer data) {

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		SS_CORE_ASSERT(data.Size == m_Config.Width * m_Config.Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_TextureHandle, 0, 0, 0, m_Config.Width, m_Config.Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t bindingPoint) const {

		glBindTextureUnit(bindingPoint, m_TextureHandle);
	}



	//========================================================================================================================================



	//OpenGLTextureCube::OpenGLTextureCube(const TextureConfig& config, const std::array<ByteBuffer, 6>& data)
	//	: m_Config(config) 
	//{
	//
	//	m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(config.Format);
	//	m_DataFormat = Utils::SosoImageFormatToGLDataFormat(config.Format);
	//
	//	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureHandle);
	//	
	//	int maxMipLevels = Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height);
	//	
	//	glTextureStorage2D(m_TextureHandle, maxMipLevels, m_InternalFormat, m_Config.Width, m_Config.Height);
	//	
	//	glTextureParameteri(m_TextureHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	//	glTextureParameteri(m_TextureHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTextureParameteri(m_TextureHandle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//
	//
	//	// Upload all six faces of the cube map
	//	SetData(data);
	//	glGenerateTextureMipmap(m_TextureHandle);
	//	m_IsLoaded = true;
	//}

	OpenGLTextureCube::OpenGLTextureCube(const TextureConfig& config, ByteBuffer data)
		: m_Config(config)
	{

		// Initialize unique resource ID
		ID = {};

		SS_CORE_ASSERT(!data.Data, "");

		GLenum internalFormat = Utils::SosoImageFormatToGLInternalFormat(m_Config.Format);
		GLenum dataFormat = Utils::SosoImageFormatToGLDataFormat(m_Config.Format);

		// Calculate mip levels
		uint32_t levels = m_Config.GenerateMips ? Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height) : 1;

		// Create empty cubemap storage
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureHandle);
		glTextureStorage2D(m_TextureHandle, levels, internalFormat, m_Config.Width, m_Config.Height);


		// Sampler Object setup
		glGenSamplers(1, &m_SamplerHandle);

		GLenum filter = Utils::GLSamplerFilter(m_Config.Filter);
		GLenum wrap = Utils::GLSamplerWrap(m_Config.Wrap);


		// Set filtering
		if (m_Config.GenerateMips) {
			glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_MIN_FILTER, (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
			glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_MAG_FILTER, filter);
		}
		else {
			glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_MIN_FILTER, filter);
			glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_MAG_FILTER, filter);
		}

		// Set wrap
		glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_WRAP_S, wrap);
		glSamplerParameteri(m_SamplerHandle, GL_TEXTURE_WRAP_T, wrap);

		// Enable max quality anisotropic filtering
		float maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
		glSamplerParameterf(m_SamplerHandle, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

		GLenum type = (m_Config.Format == ImageFormat::RGBA32F || m_Config.Format == ImageFormat::RGBA16F) ? GL_FLOAT : GL_UNSIGNED_BYTE;

		m_IsLoaded = true;
	}

	//void OpenGLTextureCube::SetData(const std::array<ByteBuffer, 6>& data) {
	//
	//	
	//	auto memorySize = Utils::GetMemorySize(m_Config.Format, m_Config.Width, m_Config.Height);
	//
	//	//(order: +X, -X, +Y, -Y, +Z, -Z)
	//	for (uint32_t i = 0; i < 6; i++) {
	//		SS_CORE_ASSERT(data[i].Size == memorySize, "Data for cube face must cover entire texture");
	//		// Specify the cube face via the zoffset parameter (each face is a layer)
	//		glTextureSubImage3D(m_TextureHandle, 0, 0, 0, i, m_Config.Width, m_Config.Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data[i].Data);
	//	}
	//}



	OpenGLTextureCube::~OpenGLTextureCube() {

		glDeleteTextures(1, &m_TextureHandle);
		glDeleteSamplers(1, &m_SamplerHandle);
	}

	uint32_t OpenGLTextureCube::GetNumMipLevels() const
	{
		return Utils::CalculateMipMapCount(m_Config.Width, m_Config.Height);
	}

	void OpenGLTextureCube::Bind(uint32_t bindingPoint) const {
		glBindTextureUnit(bindingPoint, m_TextureHandle);
		glBindSampler(bindingPoint, m_SamplerHandle);
	}

	void OpenGLTextureCube::GenerateMips() {

		glGenerateTextureMipmap(m_TextureHandle);
	}

}
