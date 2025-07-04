#pragma once

#include "so-so/Renderer/Texture.h"

#include <glad/glad.h>

#include <filesystem>

namespace soso {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const TextureConfig& config, const std::filesystem::path& filepath);
		OpenGLTexture2D(const TextureConfig& config, ByteBuffer data = ByteBuffer());
		virtual ~OpenGLTexture2D();

		uint32_t GetTextureHandle() const override { return m_TextureHandle; }

		ImageFormat GetFormat() const override { return m_Config.Format; }
		uint32_t GetWidth() const override { return m_Config.Width; }
		uint32_t GetHeight() const override { return m_Config.Height; }
		uint32_t GetNumMipLevels() const override;

		void SetData(ByteBuffer data);

		void Bind(uint32_t bindingPoint = 0) const override;

		void GenerateMips() override { SS_CORE_ASSERT(false, ""); return; }

		bool IsLoaded() const override { return m_IsLoaded; }

		bool operator==(const Texture& other) const {
			return m_TextureHandle == other.GetTextureHandle();
		}

	private:
		uint32_t m_TextureHandle;

		bool m_IsLoaded = false;
		std::filesystem::path m_Filepath;

		TextureConfig m_Config;
		GLenum m_InternalFormat, m_DataFormat;

		ByteBuffer m_ImageData; // Warning: This is currently released at the end of loading. 
	};


	class OpenGLTextureCube : public TextureCube {
	public:
		OpenGLTextureCube(const TextureConfig& config,  ByteBuffer data = ByteBuffer());

		virtual ~OpenGLTextureCube();

		uint32_t GetTextureHandle() const override { return m_TextureHandle; }

		ImageFormat GetFormat() const override { return m_Config.Format; }
		uint32_t GetWidth() const override { return m_Config.Width; }
		uint32_t GetHeight() const override { return m_Config.Height; }
		uint32_t GetNumMipLevels() const override;

		void Bind(uint32_t bindingPoint = 0) const override;

		void GenerateMips() override;


		bool IsLoaded() const override { return m_IsLoaded; }

	private:
		uint32_t m_TextureHandle;
		uint32_t m_SamplerHandle; 

		TextureConfig m_Config;
		bool m_IsLoaded = false;

		ByteBuffer m_ImageData; // Warning: This is currently released at the end of loading. 
	};

}