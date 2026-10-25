#pragma once

#include "so-so/Core/UID.h"

#include <cstdint>


namespace soso {
	
	enum class ResourceFlag : std::uint32_t {
		
		None = 0,
		Invalid = 1u << 0,
		Missing = 1u << 1
	};

	enum class ResourceType : std::uint32_t {

		None = 0,
		Texture2D,
		TextureCube
	};


	using ResourceID = UID;
		
	class Resource {
		friend class ResourceManager;

	public:
		ResourceID ID = 0;
		std::uint32_t Flags = (std::uint32_t)ResourceFlag::None;

		virtual ~Resource() {}

		static ResourceType GetStaticType() { return ResourceType::None; }
		virtual ResourceType GetResourceType() const { return ResourceType::None; }

		virtual bool operator==(const Resource& other) const {
			return ID == other.ID;
		}

		virtual bool operator!=(const Resource& other) const {
			return !(*this == other);
		}

	private:
		bool IsValid() const { return ((Flags & (std::uint32_t)ResourceFlag::Invalid) | (Flags & (std::uint32_t)ResourceFlag::Missing)) == 0; }
		bool IsFlagSet(ResourceFlag flag) const { return Flags & (std::uint32_t)flag; }

		void EnableFlag(ResourceFlag flag) { Flags |= (std::uint32_t)flag; }
		void DisableFlag(ResourceFlag flag) { Flags &= ~(std::uint32_t)flag; }
	};
}
