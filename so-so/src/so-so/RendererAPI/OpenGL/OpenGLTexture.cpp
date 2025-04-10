#include "sspch.h"
#include "so-so/RendererAPI/OpenGL/OpenGLTexture.h"

namespace soso {

	namespace Utils {

		static GLenum SosoImageFormatToGLDataFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
				case ImageFormat::RGBA16F: return GL_RGBA;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum SosoImageFormatToGLInternalFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_SRGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
				case ImageFormat::RGBA16F: return GL_RGBA16F;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static uint32_t CalculateMipMapCount(uint32_t width, uint32_t height) {
			uint32_t levels = 1;
			while ((width | height) >> levels)
				levels++;

			return levels;
		}

	}

	// All of this stuff needs to be rewritten a better way.
	OpenGLTexture2D::OpenGLTexture2D(const TextureConfig& config, Buffer data)
		: m_Config(config), m_Width(m_Config.Width), m_Height(m_Config.Height) {

		m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(m_Config.Format);
		m_DataFormat = Utils::SosoImageFormatToGLDataFormat(m_Config.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		int levels = 1;

		// Forcing gl_SRGB8 for now
		glTextureStorage2D(m_RendererID, levels, GL_SRGB8, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data.Data);
		glGenerateTextureMipmap(m_RendererID);

		if (data)
			SetData(data);

		m_IsLoaded = true;
	}

	OpenGLTexture2D::~OpenGLTexture2D() {

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data) {

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		SS_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {

		glBindTextureUnit(slot, m_RendererID);
	}

	//----------------------------------------------------------------------------------------------------------------

	OpenGLTextureCube::OpenGLTextureCube(const TextureConfig& config, const std::array<Buffer, 6>& data)
		: m_Config(config), m_Width(config.Width), m_Height(config.Height) {

			m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(config.Format);
			m_DataFormat = Utils::SosoImageFormatToGLDataFormat(config.Format);

			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
			
			int maxMipLevels = Utils::CalculateMipMapCount(m_Width, m_Height);
			
			glTextureStorage2D(m_RendererID, maxMipLevels, m_InternalFormat, m_Width, m_Height);
			
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


			// Upload all six faces of the cube map
			SetData(data);
			glGenerateTextureMipmap(m_RendererID);
			m_IsLoaded = true;
	}

	void OpenGLTextureCube::SetData(const std::array<Buffer, 6>& data) {

		uint32_t bpp = (m_DataFormat == GL_RGBA || m_DataFormat == GL_RGBA8 || m_DataFormat == GL_RGBA32F) ? 4 : (m_DataFormat == GL_RGB || m_DataFormat == GL_RGB8) ? 3 : 1;

		//(order: +X, -X, +Y, -Y, +Z, -Z)
		for (uint32_t i = 0; i < 6; i++) {
			SS_CORE_ASSERT(data[i].Size == m_Width * m_Height * bpp, "Data for cube face must cover entire texture");
			// Specify the cube face via the zoffset parameter (each face is a layer)
			glTextureSubImage3D(m_RendererID, 0, 0, 0, i, m_Width, m_Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data[i].Data);
		}
	}

	OpenGLTextureCube::~OpenGLTextureCube() {

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}

}