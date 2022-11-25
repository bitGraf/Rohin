#include <enpch.hpp>

#include "ResourceManager.hpp"

#include "Engine/Renderer/Texture.hpp"

namespace rh {

    namespace ResourceManager {
        // Texture Buffer
        const size_t Texture_Buffer_Length = 256 * 1024; // 256 kB
        unsigned char* Texture_Buffer;
        Catalog Textures;

        std::unordered_map<std::string, Texture2D*> LoadedTextures;

        Texture_Tag GetTexture(const std::string& tex_name) {
            if (LoadedTextures.find(tex_name) == LoadedTextures.end()) {
                ENGINE_LOG_WARN("Could not file texture {0}", tex_name);
                return { nullptr };
            }

            Texture_Tag tag;
            tag.texture = LoadedTextures.at(tex_name);

            return tag;
        }

        void RegisterTexture(const std::string& path) {
            //std::string tex_name = GetTexturePath(path);
            std::string tex_name = path;
            if (LoadedTextures.find(tex_name) == LoadedTextures.end()) {
                // not loaded yet
                void* loc = Textures.alloc();
                Texture2D* tex = Texture2D::CreateAtLocation(loc, "Data/" + path);
                LoadedTextures.emplace(tex_name, tex);
            }
        }

        void CreateBuffers() {
            Texture_Buffer = (unsigned char*)malloc(Texture_Buffer_Length);
            Textures.init(Texture_Buffer, Texture_Buffer_Length, sizeof(Texture_Tag), alignof(Texture_Tag));
        }

        void ResetBuffers() {
            Textures.free_all();
        }

        void DestroyBuffers() {
            Textures.free_all();
            free(Texture_Buffer);
        }
    }
}