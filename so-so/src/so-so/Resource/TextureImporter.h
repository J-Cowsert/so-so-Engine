#pragma once
#include "so-so/Renderer/Texture.h"
#include <filesystem>

namespace soso {

	struct ImageResult {

		ByteBuffer Buffer;
		uint32_t Width, Height;

		// Mainly for checking whether an image is HDR. If not it defualts to RGBA8
		ImageFormat Format;
	};

	class TextureImporter {
	public:

		static ImageResult LoadImageFromFile(const std::filesystem::path& path);
		static ImageResult LoadImageFromBuffer(ByteBuffer buffer);

		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& path);

		static std::shared_ptr<TextureCube> LoadTextureCube(const std::array<std::filesystem::path, 6>& paths);
		static std::shared_ptr<TextureCube> LoadTextureCube(std::initializer_list<std::filesystem::path> paths);

	};
}