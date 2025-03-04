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

	std::shared_ptr<TextureCube> TextureImporter::LoadTextureCube(const std::array<std::filesystem::path, 6>& paths) {
		std::array<Buffer, 6> faceBuffers;
		int width = 0, height = 0, channels = 0;
		
		stbi_set_flip_vertically_on_load(false);

		for (size_t i = 0; i < 6; i++) {
			std::string pathStr = paths[i].string();
			Buffer data;
			data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
			channels = 4;

			if (data.Data == nullptr) {
				SS_CORE_ERROR("TextureImporter::LoadTextureCube - Could not load texture from filepath: {}", pathStr);
				
				for (size_t j = 0; j < i; j++) {
					faceBuffers[j].Release();
				}
				return nullptr;
			}

			data.Size = width * height * channels;
			faceBuffers[i] = data;
		}

		
		TextureProvision provision;
		provision.Width = width;
		provision.Height = height;
		provision.Format = ImageFormat::RGBA8;
		provision.GenerateMips = false;

		std::shared_ptr<TextureCube> textureCube = TextureCube::Create(provision, faceBuffers);

		for (auto& buffer : faceBuffers)
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