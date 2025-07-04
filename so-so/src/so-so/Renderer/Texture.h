#pragma once

#include "so-so/Core/Core.h"
#include "so-so/Core/ByteBuffer.h"

#include "so-so/Resource/Resource.h"

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

		DEPTH24STENCIL8,
		DEPTH32F,

		RED_INTEGER
	};

	enum class SamplerFilter {

		None = 0,
		Linear,
		Nearest
	};

	enum class SamplerWrap {

		None = 0,
		Clamp,
		Repeat
	};

	struct TextureConfig {

		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		SamplerFilter SamplerFilter = SamplerFilter::Linear;
		SamplerWrap SamplerWrap = SamplerWrap::Repeat;

		bool GenerateMips = true;
	};

	class Texture : public Resource {
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetTextureHandle() const = 0;

		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetNumMipLevels() const = 0;

		virtual void Bind(uint32_t bindingPoint = 0) const = 0;

		virtual void GenerateMips() = 0;

		virtual bool IsLoaded() const = 0;
	};

	class Texture2D : public Texture {
	public:
		static std::shared_ptr<Texture2D> Create(const TextureConfig& config, ByteBuffer data = ByteBuffer());
		static std::shared_ptr<Texture2D> Create(const TextureConfig& config, const std::filesystem::path& filepath);

		static ResourceType GetStaticType() { return ResourceType::Texture2D; }
		ResourceType GetResourceType() const override { return GetStaticType(); }
	};

	class TextureCube : public Texture {
	public:
		static std::shared_ptr<TextureCube> Create(const TextureConfig& config, ByteBuffer data = ByteBuffer());
		
		static ResourceType GetStaticType() { return ResourceType::TextureCube; }
		ResourceType GetResourceType() const override { return GetStaticType(); }
	};
}