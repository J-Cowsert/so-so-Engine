#pragma once
#include "so-so/Renderer/Texture.h"
#include <filesystem>

namespace soso {

	struct ImageResult {

		ByteBuffer Buffer;
		uint32_t Width = 0, Height = 0;
		ImageFormat Format = ImageFormat::None; // Mainly for checking whether an image is HDR. If not, defualts to RGBA8
	};

	class TextureImporter {
	public:

		static ImageResult LoadImageFromFile(const std::filesystem::path& path);
		static ImageResult LoadImageFromBuffer(ByteBuffer buffer); // TODO: Test
	};
}