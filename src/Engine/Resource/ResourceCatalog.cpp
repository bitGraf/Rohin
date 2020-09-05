#include "ResourceCatalog.hpp"

ResourceCatalog::ResourceCatalog()
{
}

ResourceCatalog::~ResourceCatalog()
{
}

ResourceCatalog* ResourceCatalog::_singleton = 0;

ResourceCatalog* ResourceCatalog::GetInstance() {
    if (!_singleton) {
        _singleton = new ResourceCatalog;
    }
    return _singleton;
}

bool ResourceCatalog::Init() {
    Register(eveFileUpdate, this, (Callback)&ResourceCatalog::updateResourceFromFile);

    return true;
}

void ResourceCatalog::Destroy() {
    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}

void ResourceCatalog::createNewResource(char* name, ResourceEntryType type, bool empty) {
    ResourceEntry ent;

    ent.filename = std::string(name);
    ent.data = 0;
    ent.size = 0;
    ent.type = type;

    resources[ent.filename] = ent;
    FileSystem::GetInstance()->watchFile(ent.filename, empty);
}

bool ResourceCatalog::getResource(std::string name, ResourceEntry& ent) {
    if (resources.find(name) == resources.end()) {
        return false;
    }
    else {
        ent = resources[name];
        return true;
    }
}

void ResourceCatalog::updateResourceFromFile(void* data, u32 size) {
    std::string filename((char*)data, size);

    ResourceEntry ent;
    if (!getResource(filename, ent)) {
        Console::logError("Failed to find resource %s", filename);
    }
    else {
        switch (ent.type) {
        case resMesh:
            Console::logMessage("Updating Mesh Resource from %s", ent.filename.c_str());
            break;
        case resImage:
            Console::logMessage("Updating Image Resource from %s", ent.filename.c_str());
            break;
        case resFont:
            Console::logMessage("Updating Font Resource from %s", ent.filename.c_str());
            break;
        case resShader:
            Console::logMessage("Updating Shader Resource from %s", ent.filename.c_str());
            break;
        case resOther:
            Console::logMessage("Updating Other Resource from %s", ent.filename.c_str());
            break;
        }
    }
}

bool ResourceCatalog::loadResourceFile(std::string filename) {
    Console::logMessage("Reading resource file [%s]...", filename.c_str());
    size_t bytesRead;
    char* buffer = FileSystem::GetInstance()->readAllBytes("resources\\" + filename, bytesRead);

    if (buffer && bytesRead > 0) {
        Console::logMessage(" Succesfully read %zu kilobytes", bytesRead/1024);
    }
    else {
        Console::logError("Failed");
        return false;
    }

    char correctSig[10] = { 'm', 'c', 'f', 72, 95, 'k', 'j', 0, '\r', '\n' };

    if (strncmp(buffer, correctSig, 10) == 0) {
        char* txt_numMeshes = buffer + 93;
        char* txt_numMats = buffer + 127;
        char* txt_numEntities = buffer + 159;

        int numMeshes = atoi(txt_numMeshes);
        int numMats = atoi(txt_numMats);
        int numEntities = atoi(txt_numEntities);

        char* data = buffer + 184;
        for (int n = 0; n < numMeshes; n++) {
            if (strncmp(data, "Mesh: ", 6) != 0) {
                Console::logError("Error reading .mcf file. Expected a mesh entry and didnt get one.");
                break;
            }

            TriangleMesh mesh;

            char* dump = data += 6;
            size_t end = 0;
            while (data[end] != 0 && data[end] != 10 && data[end] != 13) {
                end++;
            }

            char* mesh_name = (char*)malloc(end + 1);
            memcpy(mesh_name, data, end);
            mesh_name[end] = 0;

            data += end + 2;

            char flag = data[0];
            data++;

            int numIndices;
            memcpy(&numIndices, data, 4);
            data += 4;
            mesh.numFaces = numIndices * 3;
            mesh.indices = MemoryPool::GetInstance()->allocBlock<int>(numIndices);
            memcpy(mesh.indices.data, data, numIndices * sizeof(int));
            data += numIndices * sizeof(int);

            int numVerts;
            memcpy(&numVerts, data, 4);
            data += 4;
            mesh.numVerts = numVerts;
            mesh.vertPositions = MemoryPool::GetInstance()->allocBlock<math::vec3>(numVerts);
            memcpy(mesh.vertPositions.data, data, numVerts * 3 * sizeof(float));
            data += numVerts * 3 * sizeof(float);

            bool hasNormals = flag & 1;
            bool hasUVs = flag & 2;
            bool hasTangents = flag & 4;
            bool hasBitangents = flag & 8;

            if (hasNormals) {
                mesh.vertNormals = MemoryPool::GetInstance()->allocBlock<math::vec3>(numVerts);
                memcpy(mesh.vertNormals.data, data, numVerts * 3 * sizeof(float));
                data += numVerts * 3 * sizeof(float);
            }

            if (hasUVs) {
                mesh.vertTexCoords = MemoryPool::GetInstance()->allocBlock<math::vec2>(numVerts);
                memcpy(mesh.vertTexCoords.data, data, numVerts * 2 * sizeof(float));
                data += numVerts * 2 * sizeof(float);
            }

            if (hasTangents) {
                mesh.vertTangents = MemoryPool::GetInstance()->allocBlock<math::vec4>(numVerts);
                memcpy(mesh.vertTangents.data, data, numVerts * 4 * sizeof(float));
                data += numVerts * 4 * sizeof(float);
            }
            else {
                // generate them
                mesh.vertTangents = MemoryPool::GetInstance()->allocBlock<math::vec4>(numVerts);
                memset(mesh.vertTangents.data, 0, numVerts * 4 * sizeof(float));
            }

            if (hasBitangents) {
                mesh.vertBitangents = MemoryPool::GetInstance()->allocBlock<math::vec3>(numVerts);
                memcpy(mesh.vertBitangents.data, data, numVerts * 3 * sizeof(float));
                data += numVerts * 3 * sizeof(float);
            }
            else {
                // generate them
                mesh.vertBitangents = MemoryPool::GetInstance()->allocBlock<math::vec3>(numVerts);
                memset(mesh.vertBitangents.data, 0, numVerts * 3 * sizeof(float));
            }

            mesh.flag = flag;
            mesh.initialized = false;

            if (meshes.find(mesh_name) == meshes.end()) {
                printf(" Mesh Entry: [%s], %d verts, %d indices, flag: %d... ", mesh_name, numVerts, numIndices, (int)flag);
                if (initializeTriangleMesh(&mesh)) {
                    printf("Initialized\n");
                    meshes[mesh_name] = mesh;
                }
                else {
                    printf("Failed to initialize\n");
                }
            }

            data += 2;
        }

        // Now do materials
        for (int n = 0; n < numMats; n++) {
            if (strncmp(data, "Material: ", 10) != 0) {
                Console::logError("Error reading .mcf file. Expected a material entry and didnt get one.");
                break;
            }

            Material mat;

            char* dump = data += 10;
            size_t end = 0;
            while (data[end] != 0 && data[end] != 10 && data[end] != 13) {
                end++;
            }

            char* mat_name = (char*)malloc(end + 1);
            memcpy(mat_name, data, end);
            mat_name[end] = 0;

            data += end + 2;

            char flag = data[0];
            data++;

            float diffuseColor[4], metallicFactor, roughnessFactor;
            memcpy(diffuseColor, data, 4 * sizeof(float)); data += 4 * sizeof(float);
            memcpy(&metallicFactor, data, sizeof(float));  data += sizeof(float);
            memcpy(&roughnessFactor, data, sizeof(float)); data += sizeof(float);

            bool hasDiffuse = flag & 1;
            bool hasNormal = flag & 2;
            bool hasAMR = flag & 4;
            bool hasEmissive = flag & 8;

            mat.baseColorTexture.data   = MemoryPool::GetInstance()->allocBlock<unsigned char>(1);
            mat.normalTexture.data      = MemoryPool::GetInstance()->allocBlock<unsigned char>(1);
            mat.amrTexture.data         = MemoryPool::GetInstance()->allocBlock<unsigned char>(1);
            mat.emissiveTexture.data    = MemoryPool::GetInstance()->allocBlock<unsigned char>(1);

            if (hasDiffuse) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.baseColorTexture.data = MemoryPool::GetInstance()->allocBlock<unsigned char>(numBytes);
                memcpy(mat.baseColorTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasNormal) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.normalTexture.data = MemoryPool::GetInstance()->allocBlock<unsigned char>(numBytes);
                memcpy(mat.normalTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasAMR) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.amrTexture.data = MemoryPool::GetInstance()->allocBlock<unsigned char>(numBytes);
                memcpy(mat.amrTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasEmissive) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.emissiveTexture.data = MemoryPool::GetInstance()->allocBlock<unsigned char>(numBytes);
                memcpy(mat.emissiveTexture.data.data, data, numBytes); data += numBytes;
            }

            mat.flag = flag;
            mat.initialised = false;

            data += 2;

            if (materials.find(mat_name) == materials.end()) {
                printf(" Material Entry: [%s] flag: %d...", mat_name, (int)flag);
                if (initializeMaterial(&mat)) {
                    printf("Initialized\n");
                    materials[mat_name] = mat;
                }
                else {
                    printf("Failed to initialize\n");
                }
            }
        }
    }
    else {
        Console::logError(" Incorrect file signature found...");
        return false;
    }

    free(buffer);
    return true;
}



bool ResourceCatalog::initializeTriangleMesh(TriangleMesh* mesh) {
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

    return true;
}

bool ResourceCatalog::initializeMaterial(Material* mat) {
    // create openGL textures based on the data in the Material
    bool result = true;

    if (mat->flag & 1) {
        result = result && genTextureFromData(&mat->baseColorTexture);
    }
    if (mat->flag & 2) {
        result = result && genTextureFromData(&mat->normalTexture);
    }
    if (mat->flag & 4) {
        result = result && genTextureFromData(&mat->amrTexture);
    }
    if (mat->flag & 8) {
        result = result && genTextureFromData(&mat->emissiveTexture);
    }

    // if any genTexture failed, result = false
    return result;
}


bool ResourceCatalog::genTextureFromData(Material_Texture* tex) {
    GLuint texID;

    //Generate OpenGL Texture
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // image data
    if (tex->data.data) {
        GLenum format;
        if (tex->nrChannels == 1)
            format = GL_RED;
        else if (tex->nrChannels == 3)
            format = GL_RGB;
        else if (tex->nrChannels == 4)
            format = GL_RGBA;
        tex->format = format;

        glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, format, GL_UNSIGNED_BYTE, tex->data.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        tex->glTexID = texID;
        return true;
    }
    else {
        texID = 0;
        tex->glTexID = texID;
        return false;
    }
}


meshRef ResourceCatalog::getMesh(std::string id) {
    if (meshes.find(id) == meshes.end()) {
        return nullptr;
    }
    else {
        return &meshes[id]; //TODO: This might not be safe.
    }
}

materialRef ResourceCatalog::getMaterial(std::string id) {
    if (materials.find(id) == materials.end()) {
        return nullptr;
    }
    else {
        return &materials[id]; //TODO: This might not be safe.
    }
}