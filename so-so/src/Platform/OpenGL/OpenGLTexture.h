#pragma once

#include "so-so/Renderer/Texture.h"

#include <glad/glad.h>

namespace soso {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const TextureProvision& provision, Buffer data = Buffer());
		~OpenGLTexture2D();

		const TextureProvision& GetProvision() const override { return m_Provision; }

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		uint32_t GetRendererID() const override { return m_RendererID; }

		void SetData(Buffer data) override;

		void Bind(uint32_t slot = 0) const override;

		bool IsLoaded() const override { return m_IsLoaded; }

		bool operator==(const Texture& other) const override {
			// need to make sure types are the same
			return m_RendererID == other.GetRendererID();
		}

	private:
		TextureProvision m_Provision;

		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};


	class OpenGLTextureCube : public TextureCube {
	public:
		OpenGLTextureCube(const TextureProvision& provision, const std::array<Buffer, 6>& data);
		virtual ~OpenGLTextureCube();

		const TextureProvision& GetProvision() const override { return m_Provision; }

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		uint32_t GetRendererID() const override { return m_RendererID; }

		void Bind(uint32_t slot = 0) const override;

		bool IsLoaded() const override { return m_IsLoaded; }
		
		bool operator==(const Texture& other) const override {
			// need to make sure types are the same
			return m_RendererID == other.GetRendererID();
		}

		void SetData(const std::array<Buffer, 6>& data);
		void SetData(Buffer data) override { SS_CORE_ASSERT(false, "Use SetData with cube map data (6 faces)"); }

	private:
		TextureProvision m_Provision;

		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}