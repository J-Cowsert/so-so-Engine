#include "sspch.h"
#include "TextureImporter.h"

#include <stb_image.h>

namespace soso {

    std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path) {

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		ByteBuffer data;

		{
			std::string pathStr = path.string();
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		if (data.Data == nullptr) {
			SS_CORE_ERROR("TextureImporter::ImportTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		data.Size = width * height * channels;

		TextureConfig config;
		config.Width = width;
		config.Height = height;
		switch (channels) {
		case 3:
			config.Format = ImageFormat::RGB8;
			break;
		case 4:
			config.Format = ImageFormat::RGBA8;
			break;
		}

		std::shared_ptr<Texture2D> texture = Texture2D::Create(config, data);
		data.Release();
		return texture;
    }

	std::shared_ptr<TextureCube> TextureImporter::LoadTextureCube(const std::array<std::filesystem::path, 6>& paths) {

		std::array<ByteBuffer, 6> faceByteBuffers;
		int width = 0, height = 0, channels = 0;
		
		stbi_set_flip_vertically_on_load(false);

		for (size_t i = 0; i < 6; i++) {
			std::string pathStr = paths[i].string();
			ByteBuffer data;
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;

			if (data.Data == nullptr) {
				SS_CORE_ERROR("TextureImporter::LoadTextureCube - Could not load texture from filepath: {}", pathStr);
				
				for (size_t j = 0; j < i; j++) {
					faceByteBuffers[j].Release();
				}
				return nullptr;
			}

			data.Size = width * height * channels;
			faceByteBuffers[i] = data;
		}

		
		TextureConfig config;
		config.Width = width;
		config.Height = height;
		config.Format = ImageFormat::RGBA8;
		config.GenerateMips = false;

		std::shared_ptr<TextureCube> textureCube = TextureCube::Create(config, faceByteBuffers);

		for (auto& buffer : faceByteBuffers)
			buffer.Release();

		return textureCube;
	}

	std::shared_ptr<TextureCube> TextureImporter::LoadTextureCube(std::initializer_list<std::filesystem::path> paths) {

		SS_CORE_ASSERT(paths.size() == 6, "TextureImporter::LoadTextureCube requires exactly 6 file paths.");
		std::array<std::filesystem::path, 6> facePaths;

		std::copy(paths.begin(), paths.end(), facePaths.begin());
		return LoadTextureCube(facePaths);
	}
}