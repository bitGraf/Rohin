#include "ResourceManager.hpp"

ResourceManager::ResourceManager() :
    m_pool(MEGABYTE)
{
    m_FileSystem = nullptr;
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::update(double dt) {

}

void ResourceManager::handleMessage(Message msg) {

}

void ResourceManager::destroy() {
    m_pool.destroy();
}

void ResourceManager::sys_create(ConfigurationManager* configMgr) {
    m_pool.create();
}

void ResourceManager::setFileSystem(FileSystem* _filesys) {
    m_FileSystem = _filesys;
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
    int defaultScene = root.defaultScene;

    /* For all the scenes in the model */
    for (tinygltf::Scene currentScene : root.scenes) {
        std::string sceneName = currentScene.name;
        
        /* For all nodes in this scene*/
        int numNodesInScene = currentScene.nodes.size();
        for (int currNodeID : currentScene.nodes) {
            tinygltf::Node currentNode = root.nodes[currNodeID];

            std::string nodeName = currentNode.name;
            std::vector<double> nodePosition = currentNode.translation;
            int meshID = currentNode.mesh;

            processMesh(&root, meshID);
        }
    }
}

void ResourceManager::processMesh(tinygltf::Model* root, int id) {
    tinygltf::Mesh mesh = root->meshes[id];

    std::string name = mesh.name;

    int numPrimitives = mesh.primitives.size();
    assert(numPrimitives == 1);

    int primID = 0;

    {
        /* Instance of Engine mesh object */
        TriangleMesh myMesh;

        /* Get accessor ID for attributes */
        int attr_posID   = mesh.primitives[primID].attributes["POSITION"];
        int attr_normID  = mesh.primitives[primID].attributes["NORMAL"];
        int attr_tanID   = mesh.primitives[primID].attributes["TANGENT"];
        int attr_texID   = mesh.primitives[primID].attributes["TEXCOORD_0"];
        int attr_bitanID = mesh.primitives[primID].attributes["BITANGENT"];

        int attr_indexID = mesh.primitives[primID].indices;

        /* Fill out TriMesh struct */
        myMesh.vertPositions = processAccessor<math::vec3>(root, attr_posID);
        myMesh.vertNormals = processAccessor<math::vec3>(root, attr_normID);
        myMesh.vertTexCoords = processAccessor<math::vec2>(root, attr_tanID);
        myMesh.vertTangents = processAccessor<math::vec3>(root, attr_texID);
        myMesh.vertBitangents = processAccessor<math::vec3>(root, attr_bitanID);

        myMesh.indices = processAccessor<index_t>(root, attr_indexID);

        /* Process the material info */
        int materialID = mesh.primitives[primID].material;

        /* Update TriangleMesh information */
        myMesh.numVerts = myMesh.vertPositions.m_numElements;
        myMesh.numFaces = myMesh.indices.m_numElements / 3;

        initializeTriangleMesh(&myMesh);

        /* Push to stack */
        meshes.push_back(myMesh);
    }
}

void ResourceManager::initializeTriangleMesh(TriangleMesh* mesh) {
    /* Perform openGL initialization of mesh */
    
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    //buffer Vertex position data;
    glGenBuffers(1, &mesh->VBOpos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBOpos);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * mesh->vertPositions.m_elementSize, &(mesh->vertPositions.data[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh->vertPositions.m_elementSize, (void*)0);
    glEnableVertexAttribArray(0);

    //buffer Vertex normal data;
    glGenBuffers(1, &mesh->VBOnorm);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBOnorm);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * mesh->vertNormals.m_elementSize, &(mesh->vertNormals.data[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, mesh->vertNormals.m_elementSize, (void*)0);
    glEnableVertexAttribArray(1);

    //buffer Vertex texture data
    glGenBuffers(1, &mesh->VBOtex);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBOtex);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * mesh->vertTexCoords.m_elementSize, &(mesh->vertTexCoords.data[0].x), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, mesh->vertTexCoords.m_elementSize, (void*)0);
    glEnableVertexAttribArray(2);

    //buffer Vertex tangent data
    glGenBuffers(1, &mesh->VBOtan);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBOtan);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * mesh->vertTangents.m_elementSize, &(mesh->vertTangents.data[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, mesh->vertTangents.m_elementSize, (void*)0);
    glEnableVertexAttribArray(3);

    //buffer Vertex bitangent data
    glGenBuffers(1, &mesh->VBObitan);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBObitan);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * mesh->vertBitangents.m_elementSize, &(mesh->vertBitangents.data[0].x), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, mesh->vertBitangents.m_elementSize, (void*)0);
    glEnableVertexAttribArray(4);

    //buffer indexing
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numFaces * mesh->indices.m_elementSize * 3, &mesh->indices.data[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

template<typename T>
DataBlock<T> ResourceManager::processAccessor(tinygltf::Model* root, int id) {
    int bufferViewID = root->accessors[id].bufferView;
    //int compType = root->accessors[id].componentType;
    size_t count = root->accessors[id].count;
    //std::vector<double> max = root->accessors[id].maxValues;
    //std::vector<double> min = root->accessors[id].minValues;
    //int type = root->accessors[id].type;

    // read buffer view
    tinygltf::BufferView buffView = root->bufferViews[bufferViewID];

    int buffID = buffView.buffer;
    size_t byteLength = buffView.byteLength;
    //size_t byteOffset = buffView.byteOffset;

    /* read buffer */
    tinygltf::Buffer buffer = root->buffers[buffID];

    //size_t buffByteLength = buffer.data.size();
    std::vector<u8> data = buffer.data;

    DataBlock<T> block = m_pool.allocBlock<T>(count, true);
    memcpy(block.data, data.data(), byteLength);

    return block;
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