#pragma once
#include "so-so/Renderer/Texture.h"
#include <filesystem>

namespace soso {

	class TextureImporter {
	public:

		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& path);
		static std::shared_ptr<TextureCube> LoadTextureCube(const std::array<std::filesystem::path, 6>& paths);
		static std::shared_ptr<TextureCube> LoadTextureCube(std::initializer_list<std::filesystem::path> paths);

	};
}