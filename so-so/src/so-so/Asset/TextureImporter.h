#pragma once
#include "so-so/Renderer/Texture.h"
#include <filesystem>

namespace soso {

	class TextureImporter {
	public:

		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};
}