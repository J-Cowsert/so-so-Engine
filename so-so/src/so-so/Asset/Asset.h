#pragma once

#include "so-so/Core/core.h"
#include "so-so/Core/UID.h"

#include <filesystem>


namespace soso {
	
	enum class AssetFlag : uint8_t {
		None = 0,
		Invalid = BIT(0),
		Missing = BIT(1)
	};

	enum class AssetType : uint16_t {
		None = 0,
		Texture,
		StaticMesh,
		Material
	};


	using AssetID = UID;
		
	class Asset {
	public:

		AssetID ID = 0;
		uint8_t Flags = (uint8_t)AssetFlag::None;

		virtual ~Asset() {}

		static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType GetAssetType() const { return AssetType::None; }

		virtual bool operator==(const Asset& other) const {
			return ID == other.ID;
		}

		virtual bool operator!=(const Asset& other) const {
			return *this != other;
		}

	private:

		bool IsValid() const { return ((Flags & (uint8_t)AssetFlag::Invalid) | (Flags & (uint8_t)AssetFlag::Missing)) == 0; }
		bool IsFlagSet(AssetFlag flag) const { return Flags & (uint8_t)flag; }

		void EnableFlag(AssetFlag flag) { Flags |= (uint8_t)flag; }
		void DisableFlag(AssetFlag flag) { Flags &= ~(uint8_t)flag; }
	};


	enum class AssetLoadStatus {
		None = 0,
		Invalid,
		Loading,
		Ready,
	};

	struct AssetMetadata {
		
		AssetID ID = 0;
		AssetType Type;
		std::filesystem::path Filepath;

		AssetLoadStatus LoadStatus = AssetLoadStatus::None;
		bool IsDataLoaded = false;
	};
}