#include "sspch.h"
#include "ResourceManager.h"

namespace soso {

    //void ResourceManager::Init() {
    //
    //
    //}
    //
    //void ResourceManager::Shutdown() {
    //
    //
    //}

   

    std::shared_ptr<Resource> ResourceManager::GetResource(const ResourceID& id) {

        return nullptr;
    }

    std::shared_ptr<Resource> ResourceManager::GetResourceFromFilepath(const std::filesystem::path& path) {

        for (auto& [id, meta] : m_Registry) {

            if (meta.Filepath == path) {

                return m_LoadedResources[id];
            }
        }

        return nullptr;
    }
}