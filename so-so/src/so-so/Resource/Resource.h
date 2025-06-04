#pragma once

#include "so-so/Core/Core.h"
#include "so-so/Core/UID.h"

#include <filesystem>


namespace soso {
	
	enum class ResourceFlag : uint32_t {
		
		None = 0,
		Invalid = BIT(0),
		Missing = BIT(1)
	};

	enum class ResourceType : uint32_t {

		None = 0,
		Shader,
		Texture,
		Mesh,
		Material
	};


	using ResourceID = UID;
		
	class Resource {
	public:
		ResourceID ID = 0;
		uint32_t Flags = (uint32_t)ResourceFlag::None;

		virtual ~Resource() {}

		static ResourceType GetStaticType() { return ResourceType::None; }
		virtual ResourceType GetResourceType() const { return ResourceType::None; }

		virtual bool operator==(const Resource& other) const {
			return ID == other.ID;
		}

		virtual bool operator!=(const Resource& other) const {
			return *this != other;
		}

	private:
		bool IsValid() const { return ((Flags & (uint32_t)ResourceFlag::Invalid) | (Flags & (uint32_t)ResourceFlag::Missing)) == 0; }
		bool IsFlagSet(ResourceFlag flag) const { return Flags & (uint32_t)flag; }

		void EnableFlag(ResourceFlag flag) { Flags |= (uint32_t)flag; }
		void DisableFlag(ResourceFlag flag) { Flags &= ~(uint32_t)flag; }
	};
}