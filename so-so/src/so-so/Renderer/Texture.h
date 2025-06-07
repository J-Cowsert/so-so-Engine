#pragma once

#include "so-so/Core/Core.h"
#include "so-so/Core/ByteBuffer.h"

#include <string>
#include <filesystem>

namespace soso {

	enum class ImageFormat {

		None = 0,
		R8,
		RGB8,
		RGBA8, 
		RGBA16F,
		RGBA32F,

		SRGB,
		SRGBA,
	};

	struct TextureConfig {
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture {
	public:
		virtual ~Texture() = default;

		virtual const TextureConfig& GetConfig() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(ByteBuffer data) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture {
	public:
		static std::shared_ptr<Texture2D> Create(const TextureConfig& config, ByteBuffer data = ByteBuffer());
		static std::shared_ptr<Texture2D> Create(const TextureConfig& config, const std::filesystem::path& filepath);
	};

	class TextureCube : public Texture {
	public:
		static std::shared_ptr<TextureCube> Create(const TextureConfig& config, const std::array<ByteBuffer, 6>& data);
	};
}