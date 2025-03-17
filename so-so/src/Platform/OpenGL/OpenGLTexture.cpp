#include "sspch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace soso {

	namespace Utils {

		static GLenum SosoImageFormatToGLDataFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
				case ImageFormat::RGBA32F: return GL_RGBA;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum SosoImageFormatToGLInternalFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
				case ImageFormat::RGBA32F: return GL_RGBA32F;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProvision& provision, Buffer data)
		: m_Provision(provision), m_Width(m_Provision.Width), m_Height(m_Provision.Height) {

		m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(m_Provision.Format);
		m_DataFormat = Utils::SosoImageFormatToGLDataFormat(m_Provision.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
			SetData(data);
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







	OpenGLTextureCube::OpenGLTextureCube(const TextureProvision& provision, const std::array<Buffer, 6>& data)
		: m_Provision(provision), m_Width(provision.Width), m_Height(provision.Height) {

			m_InternalFormat = Utils::SosoImageFormatToGLInternalFormat(provision.Format);
			m_DataFormat = Utils::SosoImageFormatToGLDataFormat(provision.Format);

			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
			
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// Upload all six faces of the cube map
			SetData(data);
			m_IsLoaded = true;
	}

	OpenGLTextureCube::~OpenGLTextureCube() {

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTextureCube::SetData(const std::array<Buffer, 6>& data) {

		uint32_t bpp = (m_DataFormat == GL_RGBA || m_DataFormat == GL_RGBA8 || m_DataFormat == GL_RGBA32F) ? 4 : (m_DataFormat == GL_RGB || m_DataFormat == GL_RGB8) ? 3 : 1;

		//(order: +X, -X, +Y, -Y, +Z, -Z)
		for (uint32_t i = 0; i < 6; i++) {
			SS_CORE_ASSERT(data[i].Size == m_Width * m_Height * bpp, "Data for cube face must cover entire texture!");
			// Specify the cube face via the zoffset parameter (each face is a layer)
			glTextureSubImage3D(m_RendererID, 0, 0, 0, i, m_Width, m_Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data[i].Data);
		}
	}
}