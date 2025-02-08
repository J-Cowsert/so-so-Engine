#include "sspch.h"
#include "TextureImporter.h"
#include <stb_image.h>

namespace soso {

    std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path) {

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		Buffer data;

		{
			std::string pathStr = path.string();
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		if (data.Data == nullptr) {
			SS_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		// TODO: think about this
		data.Size = width * height * channels;

		TextureProvision provision;
		provision.Width = width;
		provision.Height = height;
		switch (channels) {
		case 3:
			provision.Format = ImageFormat::RGB8;
			break;
		case 4:
			provision.Format = ImageFormat::RGBA8;
			break;
		}

		std::shared_ptr<Texture2D> texture = Texture2D::Create(provision, data);
		data.Release();
		return texture;
    }
}