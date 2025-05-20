#pragma once

#include "so-so/Core/core.h"

#include "Resource.h"

#include <unordered_map>

namespace soso {

	enum class ResourceLoadStatus {
		None = 0,
		Invalid,
		Loading,
		Ready
	};

	struct ResourceMetadata {

		ResourceID ID = 0;
		ResourceType Type;
		std::filesystem::path Filepath;

		ResourceLoadStatus LoadStatus = ResourceLoadStatus::None;
	};

	// Add logging and asserts later. This class needs to be robust & should eventually be moved to it's own file
	//
	class ResourceRegistry {
	public:

		const ResourceMetadata& Get(const ResourceID id) const {
			return m_Registry.at(id);
		}

		void Set(const ResourceID& id, ResourceMetadata metadata) {
			m_Registry[id] = metadata;
		}
		
		bool Contains(const ResourceID& id) {
			return m_Registry.find(id) != m_Registry.end();
		}

		size_t Size() const {
			return m_Registry.size();
		}


	private:
		std::unordered_map<ResourceID, ResourceMetadata> m_Registry;
	};

	// In the future this shouldn't be static. Each project should have an instance
	// 
	// look into std::exception for this class
	class ResourceManager {
	public:
		static void Init();
		static void Shutdown();

		static bool IsResourceValid(ResourceID id);
		
		static std::vector<ResourceID> GetAllResourcesOfType(ResourceType type);
		static std::shared_ptr<Resource> GetResource(const ResourceID& id);
		static ResourceID GetResource(const std::filesystem::path& path);
		static ResourceID LoadResource(const std::filesystem::path& path, ResourceType type);

	private:
		static std::unordered_map<ResourceID, std::shared_ptr<Resource>> s_Resources;
		static ResourceRegistry s_ResourceRegistry;
	};
}