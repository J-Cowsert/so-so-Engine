#pragma once

#include "so-so/Renderer/Texture.h"

#include <glad/glad.h>

namespace soso {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const TextureProvision& provision, Buffer data = Buffer());
		virtual ~OpenGLTexture2D();

		virtual const TextureProvision& GetProvision() const override { return m_Provision; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData(Buffer data) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override {
			return m_RendererID == other.GetRendererID();
		}
	private:
		TextureProvision m_Provision;

		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}