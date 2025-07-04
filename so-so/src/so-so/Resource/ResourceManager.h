#pragma once

#include "so-so/Core/Core.h"

#include "Resource.h"

#include "so-so/Core/Profiler.h"

#include <unordered_map>
#include <type_traits>

namespace soso {

	/*
		WIP

		Need serialization
	*/

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

		//ResourceLoadStatus LoadStatus = ResourceLoadStatus::None;
	};
	
	class ResourceManager {
	public:
		//static void Init();
		//static void Shutdown();

		static bool IsResourceValid(ResourceID id);

		static std::shared_ptr<Resource> GetResource(const ResourceID& id);
		static std::shared_ptr<Resource> GetResourceFromFilepath(const std::filesystem::path& path);
		
		template<typename T, typename...Args>
		static std::shared_ptr<T> CreateResourceFromFile(Args&&...args) {

			SS_PROFILE_FUNCTION();
			static_assert(std::is_base_of<Resource, T>::value, "Must be a subclass of Resource");

			std::shared_ptr<T> resource = T::Create(std::forward<Args>(args)...);
			
			auto tup = std::forward_as_tuple(std::forward<Args>(args)...);
			auto& path = std::get<std::filesystem::path&>(tup);

			ResourceMetadata meta;
			meta.Filepath = path;
			meta.ID = resource->ResourceID;
			meta.Type = resource->GetResourceType();

			SS_CORE_ASSERT(meta.ID != 0, "");

			m_Registry[meta.ID] = meta;
			m_LoadedResources[meta.ID] = resource;

			return resource;
		}

	private:
		inline static std::unordered_map<ResourceID, std::shared_ptr<Resource>> m_LoadedResources;
		inline static std::unordered_map<ResourceID, ResourceMetadata> m_Registry;
	};
}