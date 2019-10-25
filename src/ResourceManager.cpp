#include "ResourceManager.hpp"

ResourceManager::ResourceManager() {
    m_FileSystem = nullptr;
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::update(double dt) {

}

void ResourceManager::handleMessage(Message msg) {

}

void ResourceManager::destroy() {
    ResourceList* head = m_resourceList.next;
    ResourceList* next = nullptr;

    while (head != nullptr) {
        // save pointer to next element 
        // before deleting this element
        next = head->next;

        freeResource(&head->resource);
        free(head);

        head = next;
        next = nullptr;
    }

    // done stepping through the list
    // free the root resource.
    freeResource(&m_resourceList.resource);
}

void ResourceManager::sys_create(ConfigurationManager* configMgr) {
    m_resourceList.resource.data = nullptr;
    m_resourceList.next = nullptr;

    m_tail = &m_resourceList;
}

void ResourceManager::setFileSystem(FileSystem* _filesys) {
    m_FileSystem = _filesys;
}

/*void ResourceManager::setMemoryManager(MemoryManager* _memManager) {
    m_memoryManager = _memManager;
}*/

void ResourceManager::createNewResource(stringID id) {
    ResourceList* res = new ResourceList;
    res->next = nullptr;
    res->resource.data = nullptr;
    res->resource.ID = id;

    assert(m_tail->next == nullptr);

    m_tail->next = res;
    m_tail = m_tail->next;
}

void ResourceManager::printAllResources() {
    int numResources = 0;
    ResourceList* next = m_resourceList.next;
    while (next != nullptr) {
        // next is a valid entry in the list
        printf("ID: %d\n", next->resource.ID);

        next = next->next;
        
        numResources++;
    }
    printf("%d resources cycled.\n", numResources);
}

void ResourceManager::freeResource(Resource* res) {
    res->ID = 0;
    free(res->data);
    res->data = nullptr;
}

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

    /* Start parsing file */

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
            auto normalsID   = mesh.primitives[0].attributes["NORMAL"];
            auto tangentsID  = mesh.primitives[0].attributes["TANGENT"];
            auto texCoord0ID = mesh.primitives[0].attributes["TEXCOORD_0"];
            auto indicesID   = mesh.primitives[0].indices;
            auto materialID  = mesh.primitives[0].material;

            /* Access Mesh data */
            /* Use root.accessors[positionsID].type to determine what dataType for each array*/
            float* posData = (float*)readAccessor(&root, positionsID);
            float* normalsData = (float*)readAccessor(&root, normalsID);
            float* tangentsData = (float*)readAccessor(&root, tangentsID);
            float* texCoordsData = (float*)readAccessor(&root, texCoord0ID);
            int* indicesData = (int*)readAccessor(&root, indicesID);

            /* Access material data */
            auto material = root.materials[materialID];
            printf("Material: %s\n", material.name.c_str());

            auto bcf = material.pbrMetallicRoughness.baseColorFactor;
            auto mf  = material.pbrMetallicRoughness.metallicFactor;
            auto rf = material.pbrMetallicRoughness.roughnessFactor;
            auto bct = material.pbrMetallicRoughness.baseColorTexture;

            auto bci = bct.index;
            auto bctx = bct.texCoord;

            auto texture = root.textures[bci];
            auto texID = texture.source;

            /* read textures */
            void* image = readImage(&root, texID);

            /* Delete everything I just allocated */
            free(posData);
            free(normalsData);
            free(tangentsData);
            free(texCoordsData);
            free(indicesData);
            free(image);
        }
    }
}

void* ResourceManager::readAccessor(tinygltf::Model* root, int accessorID) {
    auto bufferViewID   = root->accessors[accessorID].bufferView;
    auto compType       = root->accessors[accessorID].componentType;
    auto count          = root->accessors[accessorID].count;
    auto max            = root->accessors[accessorID].maxValues;
    auto min            = root->accessors[accessorID].minValues;
    auto type           = root->accessors[accessorID].type;

    printf("Accessor: %d %d %zu %d\n",
        bufferViewID, compType, count, type);

    // read buffer view
    auto buffView = root->bufferViews[bufferViewID];

    auto buffID = buffView.buffer;
    auto byteLength = buffView.byteLength;
    auto byteOffset = buffView.byteOffset;

    // read buffer
    auto buffer = root->buffers[buffID];

    auto buffByteLength = buffer.data.size();
    auto data = buffer.data;

    // alloc data to raw data <- should get from memoryManager
    void* out_data = malloc(byteLength);
    memcpy(out_data, data.data() + byteOffset, byteLength);
    return out_data;
}

void* ResourceManager::readImage(tinygltf::Model* root, int imageID) {
    auto bufferViewID = root->images[imageID].bufferView;

    // read buffer view
    auto buffView = root->bufferViews[bufferViewID];

    auto buffID = buffView.buffer;
    auto byteLength = buffView.byteLength;
    auto byteOffset = buffView.byteOffset;

    // read buffer
    auto buffer = root->buffers[buffID];

    auto buffByteLength = buffer.data.size();
    auto data = buffer.data;

    // alloc data to raw data <- should get from memoryManager
    void* out_data = malloc(byteLength);
    memcpy(out_data, data.data() + byteOffset, byteLength);
    return out_data;
}