#include "ResourceManager.hpp"

ResourceManager::ResourceManager() :
    m_pool(4 * MEGABYTE)
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

CoreSystem* ResourceManager::create() {


    m_pool.create();

    return this;
}

void ResourceManager::setFileSystem(FileSystem* _filesys) {
    m_FileSystem = _filesys;
}

void ResourceManager::loadModelFromFile(std::string path, bool binary) {
    tinygltf::Model root;
    std::string err;
    std::string warn;

    stbi_set_flip_vertically_on_load(false);

    bool ret;
    if (binary) {
        ret = loader.LoadBinaryFromFile(&root, &err, &warn, path);
    } else {
        ret = loader.LoadASCIIFromFile(&root, &err, &warn, path);
    }

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

    /* Read all materials from file */  
    for (tinygltf::Material currMat : root.materials) {
        Material myMat;

        // Material name
        myMat.name = currMat.name;
        
        // Extract texture data
        myMat.emissiveTexture.index = currMat.emissiveTexture.index;
        myMat.emissiveTexture.tex_coord = currMat.emissiveTexture.texCoord;
        initializeTexture(&root, &myMat.emissiveTexture);

        myMat.normalTexture.index = currMat.normalTexture.index;
        myMat.normalTexture.tex_coord = currMat.normalTexture.texCoord;
        myMat.normalTexture.value = static_cast<f32>(currMat.normalTexture.scale);
        initializeTexture(&root, &myMat.normalTexture);

        myMat.occlusionTexture.index = currMat.occlusionTexture.index;
        myMat.occlusionTexture.tex_coord = currMat.occlusionTexture.texCoord;
        myMat.occlusionTexture.value = static_cast<f32>(currMat.occlusionTexture.strength);
        initializeTexture(&root, &myMat.occlusionTexture);

        myMat.baseColorTexture.index = currMat.pbrMetallicRoughness.baseColorTexture.index;
        myMat.baseColorTexture.tex_coord = currMat.pbrMetallicRoughness.baseColorTexture.texCoord;
        initializeTexture(&root, &myMat.baseColorTexture);

        myMat.metallicRoughnessTexture.index = currMat.pbrMetallicRoughness.metallicRoughnessTexture.index;
        myMat.metallicRoughnessTexture.tex_coord = currMat.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
        initializeTexture(&root, &myMat.metallicRoughnessTexture);

        // Material Factors
        myMat.emissiveFactor = math::vec3(currMat.emissiveFactor.data());
        myMat.baseColorFactor = math::vec4(currMat.pbrMetallicRoughness.baseColorFactor.data());
        myMat.metallicFactor = currMat.pbrMetallicRoughness.metallicFactor;
        myMat.roughnessFactor = currMat.pbrMetallicRoughness.roughnessFactor;
        
        // Other values
        myMat.alphaCutoff = currMat.alphaCutoff;
        myMat.alphaMode = currMat.alphaMode;
        myMat.doubleSided = currMat.doubleSided;

        materials[myMat.name] = myMat;
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
        myMesh.vertTexCoords = processAccessor<math::vec2>(root, attr_texID);
        myMesh.vertTangents = processAccessor<math::vec4>(root, attr_tanID);
        myMesh.vertBitangents = processAccessor<math::vec3>(root, attr_bitanID);

        myMesh.indices = processAccessor<index_t>(root, attr_indexID);

        /* Process the material info */
        int materialID = mesh.primitives[primID].material;

        /* Update TriangleMesh information */
        myMesh.numVerts = myMesh.vertPositions.m_numElements;
        myMesh.numFaces = myMesh.indices.m_numElements / 3;

        initializeTriangleMesh(&myMesh);

        /* Push to stack */
        meshes[name] = myMesh;
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
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mesh->vertTangents.m_elementSize, (void*)0);
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

void ResourceManager::initializeTexture(tinygltf::Model* root, Material_Texture* mTex) {
    if (mTex->index >= 0) {
        tinygltf::Texture tex = root->textures[mTex->index];
        std::string texName = tex.name;

        /* Check for sampler info */
        int texSampler = tex.sampler;
        if (texSampler >= 0) {

        }

        int texSource = tex.source;
        if (texSource >= 0) {
            tinygltf::Image image = root->images[texSource];

            std::string imageName = image.name;
            std::string imageMimeType = image.mimeType;
            int bufferViewID = image.bufferView;

            tinygltf::BufferView bv = root->bufferViews[bufferViewID];

            tinygltf::Buffer buffer = root->buffers[bv.buffer];
            size_t byteLength = bv.byteLength;
            size_t byteOffset = bv.byteOffset;

            //DataBlock<u8> block = m_pool.allocBlock<u8>(byteLength, true);
            //memcpy(block.data, buffer.data.data() + byteOffset, byteLength);

            /* now have the texture data in block */
            mTex->width = image.width;
            mTex->height = image.height;
            mTex->channelBits = image.bits;
            mTex->nrChannels = image.component;

            GLint internalFormat = -1;
            GLenum format = -1;

            switch (image.component) {
            case 1: {
                switch (image.bits) {
                case 8:  {internalFormat = GL_R8;  } break;
                case 16: {internalFormat = GL_R16; } break;
                }
                format = GL_RED;
            } break;
            case 2: {
                switch (image.bits) {
                case 8: {internalFormat = GL_RG8;  } break;
                case 16: {internalFormat = GL_RG16; } break;
                }
                format = GL_RG;
            } break;
            case 3: {
                switch (image.bits) {
                case 8: {internalFormat = GL_RGB8;  } break;
                case 16: {internalFormat = GL_RGB16; } break;
                }
                format = GL_RGB;
            } break;
            case 4: {
                switch (image.bits) {
                case 8: {internalFormat = GL_RGBA8;  } break;
                case 16: {internalFormat = GL_RGBA16; } break;
                }
                format = GL_RGBA;
            } break;
            }

            if (format < 0 || internalFormat < 0) {
                printf("Failed to decipher pixel format\n");
                assert(false);
            }

            mTex->format = format;
            mTex->internalFormat = internalFormat;

            glGenTextures(1, &mTex->glTexID);
            glBindTexture(GL_TEXTURE_2D, mTex->glTexID);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mTex->width, mTex->height, 0, format, GL_UNSIGNED_BYTE, image.image.data());
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }
}



template<typename T>
DataBlock<T> ResourceManager::processAccessor(tinygltf::Model* root, int id) {
    int bufferViewID = root->accessors[id].bufferView;
    int compType = root->accessors[id].componentType;
    size_t count = root->accessors[id].count;
    //std::vector<double> max = root->accessors[id].maxValues;
    //std::vector<double> min = root->accessors[id].minValues;
    int type = root->accessors[id].type;

    // read buffer view
    tinygltf::BufferView buffView = root->bufferViews[bufferViewID];

    int buffID = buffView.buffer;
    size_t byteLength = buffView.byteLength;
    size_t byteOffset = buffView.byteOffset;

    /* read buffer */
    tinygltf::Buffer buffer = root->buffers[buffID];

    //size_t buffByteLength = buffer.data.size();
    std::vector<u8> data = buffer.data;

    DataBlock<T> block = m_pool.allocBlock<T>(count, true);
    memcpy(block.data, data.data()+byteOffset, byteLength);

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

meshRef ResourceManager::getMesh(std::string id) {
    if (meshes.find(id) == meshes.end()) {
        return nullptr;
    } else {
        return &meshes[id]; //TODO: This might not be safe.
    }
}

materialRef ResourceManager::getMaterial(std::string id) {
    if (materials.find(id) == materials.end()) {
        return nullptr;
    }
    else {
        return &materials[id]; //TODO: This might not be safe.
    }
}



void ResourceManager::createGrid(f32 lineSep, u32 numLines, f32 gridSize) {
    using namespace math;

    f32 height = 0;

    std::vector<vec3> verts;

    s32 range = (numLines-1) / 2;
    for (s32 n = -range; n <= range; n++) {
        vec3 v1(-gridSize,  height,  n*lineSep);
        vec3 v2( gridSize,  height,  n*lineSep);
        vec3 v3( n*lineSep, height, -gridSize);
        vec3 v4( n*lineSep, height,  gridSize);

        verts.push_back(v1);
        verts.push_back(v2);
        verts.push_back(v3);
        verts.push_back(v4);
    }

    numGridVerts = verts.size();

    /* Perform openGL initialization of mesh */
    gridVAO = 0;
    GLuint gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    //buffer Vertex position data;
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec3), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}