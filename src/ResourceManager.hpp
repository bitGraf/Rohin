#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <cassert>
#include <unordered_map>

#include "CoreSystem.hpp"
#include "FileSystem.hpp"
#include "MemoryManager.hpp"
#include "Models.hpp"
#include "Material.hpp"

#include "tiny_gltf.h"

const u32 KILOBYTE = 1024;              // 1 KB worth of bytes
const u32 MEGABYTE = 1024 * KILOBYTE;   // 1 MB worth of bytes

class ResourceManager : public CoreSystem {
public:
    ResourceManager();
    ~ResourceManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    void sys_create(ConfigurationManager* configMgr);

    void setFileSystem(FileSystem* _filesys);

    void loadModelFromFile(std::string path);

    meshRef getMesh(std::string);
    materialRef getMaterial(std::string);

    void createGrid(f32 lineSep, u32 numLines, f32 gridSize);
    GLuint gridVAO;
    GLuint numGridVerts;

private:
    FileSystem*     m_FileSystem;
    //MemoryManager*  m_memoryManager;
    PoolAllocator m_pool;

    tinygltf::TinyGLTF loader;

    void* readAccessor(tinygltf::Model* root, int accessorID);
    void* readImage(tinygltf::Model* root, int imageID);

    std::unordered_map<std::string, TriangleMesh> meshes;
    std::unordered_map<std::string, Material> materials;

    void processMesh(tinygltf::Model* root, int id);

    template<typename T>
    DataBlock<T> processAccessor(tinygltf::Model* root, int id);
    void initializeTriangleMesh(TriangleMesh* mesh);
    void initializeTexture(tinygltf::Model* root, Material_Texture* mTex);
};



/*
void ResourceManager::loadModelFromFile(std::string path) {
    tinygltf::Model root;
    std::string err;
    std::string warn;

    //    bool ret = loader.LoadBinaryFromFile(
    //        &model, &err, &warn, path);
    bool ret = loader.LoadASCIIFromFile(
        &root, &err, &warn, path);

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }

    auto currScene = root.defaultScene;

    // Loop through all scenes
    for (auto scene : root.scenes) {
        auto sceneName = scene.name;
        printf("Parsing scene: %s\n", sceneName.c_str());

        // For every scene, loop through all nodes
        for (auto nodeID : scene.nodes) {
            auto node = root.nodes[nodeID];
            auto nodeName = node.name;
            printf("Parsing node: %s\n", nodeName.c_str());

            auto nodePos = node.translation;
            auto nodeMeshID = node.mesh;

            auto mesh = root.meshes[nodeMeshID];

            auto meshName = mesh.name;
            printf("Parsing mesh: %s\n", meshName.c_str());

            // Extract mesh primitive data indices
            auto positionsID = mesh.primitives[0].attributes["POSITION"];
            auto normalsID = mesh.primitives[0].attributes["NORMAL"];
            auto tangentsID = mesh.primitives[0].attributes["TANGENT"];
            auto texCoord0ID = mesh.primitives[0].attributes["TEXCOORD_0"];
            auto indicesID = mesh.primitives[0].indices;
            auto materialID = mesh.primitives[0].material;

            // Use root.accessors[positionsID].type to determine what dataType for each array
            float* posData = (float*)readAccessor(&root, positionsID);
            float* normalsData = (float*)readAccessor(&root, normalsID);
            float* tangentsData = (float*)readAccessor(&root, tangentsID);
            float* texCoordsData = (float*)readAccessor(&root, texCoord0ID);
            int* indicesData = (int*)readAccessor(&root, indicesID);

            auto material = root.materials[materialID];
            printf("Material: %s\n", material.name.c_str());

            auto bcf = material.pbrMetallicRoughness.baseColorFactor;
            auto mf = material.pbrMetallicRoughness.metallicFactor;
            auto rf = material.pbrMetallicRoughness.roughnessFactor;
            auto bct = material.pbrMetallicRoughness.baseColorTexture;

            auto bci = bct.index;
            auto bctx = bct.texCoord;

            auto texture = root.textures[bci];
            auto texID = texture.source;

            void* image = readImage(&root, texID);

            free(posData);
            free(normalsData);
            free(tangentsData);
            free(texCoordsData);
            free(indicesData);
            free(image);
        }
    }
}
*/

#endif
