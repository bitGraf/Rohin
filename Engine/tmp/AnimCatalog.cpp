#include <enpch.hpp>
#include "AnimCatalog.hpp"

namespace Engine {

    namespace AnimCatalog {
        // ANIM_HOOK std::unordered_map<std::string, md5::Animation*> m_AnimList;

        // ANIM_HOOK void Register(const std::string& anim_name, const std::string& filepath, FileFormat file_type) {
        // ANIM_HOOK     switch (file_type) {
        // ANIM_HOOK     case FileFormat::MD5_Text: {
        // ANIM_HOOK         md5::Animation* anim = new md5::Animation();
        // ANIM_HOOK         md5::LoadMD5AnimFile(filepath, anim);
        // ANIM_HOOK 
        // ANIM_HOOK         if (true /*anim->Loaded()*/) { // TODO: check if animation was loaded correctly
        // ANIM_HOOK             m_AnimList[anim_name] = anim;
        // ANIM_HOOK             ENGINE_LOG_INFO("Registering Animation: [{0}] from [{1}]", anim_name, filepath);
        // ANIM_HOOK         }
        // ANIM_HOOK     }break;
        // ANIM_HOOK     case FileFormat::NBT_Basic: {
        // ANIM_HOOK         ENGINE_LOG_ERROR("Proper Animation FileFormat not specified!");
        // ANIM_HOOK         /*
        // ANIM_HOOK         Mesh* mesh = new Mesh(filepath);
        // ANIM_HOOK         if (mesh->Loaded()) {
        // ANIM_HOOK             m_MeshList[anim_name] = mesh;
        // ANIM_HOOK             ENGINE_LOG_INFO("Registering Animation: [{0}] from [{1}]", anim_name, filepath);
        // ANIM_HOOK         }
        // ANIM_HOOK         */
        // ANIM_HOOK     }break;
        // ANIM_HOOK     case FileFormat::None: {
        // ANIM_HOOK         ENGINE_LOG_ERROR("Proper Animation FileFormat not specified!");
        // ANIM_HOOK     }break;
        // ANIM_HOOK     }
        // ANIM_HOOK }
        // ANIM_HOOK 
        // ANIM_HOOK md5::Animation* Get(const std::string& anim_name) {
        // ANIM_HOOK     if (m_AnimList.find(anim_name) == m_AnimList.end()) {
        // ANIM_HOOK         return nullptr;
        // ANIM_HOOK     }
        // ANIM_HOOK 
        // ANIM_HOOK     return m_AnimList[anim_name];
        // ANIM_HOOK }
        // ANIM_HOOK 
        // ANIM_HOOK void Create() {
        // ANIM_HOOK 
        // ANIM_HOOK }
        // ANIM_HOOK 
        // ANIM_HOOK void Destroy() {
        // ANIM_HOOK     // deallocate memory
        // ANIM_HOOK     for (const auto& it : m_AnimList) {
        // ANIM_HOOK         delete it.second;
        // ANIM_HOOK     }
        // ANIM_HOOK     // Clear the mesh catalog of all entries
        // ANIM_HOOK     m_AnimList.clear();
        // ANIM_HOOK }
    }
}