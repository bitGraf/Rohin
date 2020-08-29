#include "ResourceManager.hpp"

//ResourceManager g_ResourceManager;

ResourceManager::ResourceManager() :
    m_pool(64 * MEGABYTE)
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::update(double dt) {

}

void ResourceManager::handleMessage(Message msg) {


    m_FileSystem.handleMessage(msg);
}

void ResourceManager::destroy() {
    m_pool.destroy();
    m_FileSystem.destroy();
}

CoreSystem* ResourceManager::create() {
    m_FileSystem.create();

    m_pool.create();

    // watch resource locations
    printf("Searching for resource files... Found:\n");
    std::vector<std::string> allFiles = m_FileSystem.getAllFilesOnPath("resources");
    std::vector<std::string> resourceFiles;
    int numLoaded = 0;
    for (auto file : allFiles) {
        // check if file is a .mcf
        auto k = file.find_last_of('.');
        if (k != std::string::npos) {
            auto ending = file.data() + k;
            if (strcmp(ending, ".mcf") == 0) {
                // this is a mesh catalog file.
                resourceFiles.push_back(file);

                loadResourceFile(file);
                numLoaded++;
            }
        }
    }
    printf("Loaded %d resources files\n", numLoaded);
    u32 left = m_pool.getBytesLeft();
    u32 total = m_pool.getTotalBytes();
    printf("Memory pool: %.1fmB/%.1fmB (%.2f%%)\n", (float)left/MEGABYTE, (float)total/MEGABYTE, (float)left/total * 100);

    return this;
}

void ResourceManager::loadResourceFile(std::string filename) {
    printf("  Reading resource file [%s]...", filename.c_str());
    size_t bytesRead;
    char* buffer = m_FileSystem.readAllBytes("resources\\" + filename, bytesRead);

    if (buffer && bytesRead > 0) {
        printf("Succesfully read %zu bytes\n", bytesRead);
    }
    else {
        printf("Failed\n");
        return;
    }

    char correctSig[10] = { 'm', 'c', 'f', 72, 95, 'k', 'j', 0, '\r', '\n' };

    if (strncmp(buffer, correctSig, 10) == 0) {
        char* txt_numMeshes   = buffer + 93;
        char* txt_numMats     = buffer + 127;
        char* txt_numEntities = buffer + 159;

        int numMeshes   = atoi(txt_numMeshes);
        int numMats     = atoi(txt_numMats);
        int numEntities = atoi(txt_numEntities);

        char* data = buffer + 184;
        for (int n = 0; n < numMeshes; n++) {
            if (strncmp(data, "Mesh: ", 6) != 0) {
                printf("Error reading .mcf file. Expected a mesh entry and didnt get one.\n");
                break;
            }

            TriangleMesh mesh;

            char* dump = data += 6;
            size_t end = 0;
            while (data[end] != 0 && data[end] != 10 && data[end] != 13) {
                end++;
            }

            char* mesh_name = (char*)malloc(end+1);
            memcpy(mesh_name, data, end);
            mesh_name[end] = 0;

            data += end+2;

            char flag = data[0];
            data++;

            int numIndices;
            memcpy(&numIndices, data, 4);
            data += 4;
            mesh.numFaces = numIndices * 3;
            mesh.indices = reserveDataBlocks<int>(numIndices);
            memcpy(mesh.indices.data, data, numIndices * sizeof(int));
            //int* mesh_indices = (int*)malloc(numIndices * sizeof(int));
            //memcpy(mesh_indices, data, numIndices * sizeof(int));
            data += numIndices * sizeof(int);

            int numVerts;
            memcpy(&numVerts, data, 4);
            data += 4;
            mesh.numVerts = numVerts;
            mesh.vertPositions = reserveDataBlocks<math::vec3>(numVerts);
            memcpy(mesh.vertPositions.data, data, numVerts * 3 * sizeof(float));
            //float* mesh_verts = (float*)malloc(numVerts * 3 * sizeof(float));
            //memcpy(mesh_verts, data, numVerts * 3 * sizeof(float));
            data += numVerts * 3 * sizeof(float);

            bool hasNormals = flag & 1;
            bool hasUVs = flag & 2;
            bool hasTangents = flag & 4;
            bool hasBitangents = flag & 8;

            if (hasNormals) {
                mesh.vertNormals = reserveDataBlocks<math::vec3>(numVerts);
                memcpy(mesh.vertNormals.data, data, numVerts * 3 * sizeof(float));
                //float* mesh_normals = (float*)malloc(numVerts * 3 * sizeof(float));
                //memcpy(mesh_normals, data, numVerts * 3 * sizeof(float));
                data += numVerts * 3 * sizeof(float);
            }

            if (hasUVs) {
                mesh.vertTexCoords = reserveDataBlocks<math::vec2>(numVerts);
                memcpy(mesh.vertTexCoords.data, data, numVerts * 2 * sizeof(float));
                //float* mesh_UVs = (float*)malloc(numVerts * 2 * sizeof(float));
                //memcpy(mesh_UVs, data, numVerts * 2 * sizeof(float));
                data += numVerts * 2 * sizeof(float);
            }

            if (hasTangents) {
                mesh.vertTangents = reserveDataBlocks<math::vec4>(numVerts);
                memcpy(mesh.vertTangents.data, data, numVerts * 4 * sizeof(float));
                //float* mesh_tangents = (float*)malloc(numVerts * 4 * sizeof(float));
                //memcpy(mesh_tangents, data, numVerts * 4 * sizeof(float));
                data += numVerts * 4 * sizeof(float);
            }
            else {
                // generate them
                mesh.vertTangents = reserveDataBlocks<math::vec4>(numVerts);
                memset(mesh.vertTangents.data, 0, numVerts * 4 * sizeof(float));
            }

            if (hasBitangents) {
                mesh.vertBitangents = reserveDataBlocks<math::vec3>(numVerts);
                memcpy(mesh.vertBitangents.data, data, numVerts * 3 * sizeof(float));
                data += numVerts * 3 * sizeof(float);
            }
            else {
                // generate them
                mesh.vertBitangents = reserveDataBlocks<math::vec3>(numVerts);
                memset(mesh.vertBitangents.data, 0, numVerts * 3 * sizeof(float));
            }

            mesh.flag = flag;
            mesh.initialized = false;

            if (meshes.find(mesh_name) == meshes.end()) {
                printf("    Mesh Entry: [%s], %d verts, %d indices, flag: %d... ", mesh_name, numVerts, numIndices, (int)flag);
                initializeTriangleMesh(&mesh);
                printf("Initialized\n");
                meshes[mesh_name] = mesh;
            }

            data += 2;
        }

        // Now do materials
        for (int n = 0; n < numMats; n++) {
            if (strncmp(data, "Material: ", 10) != 0) {
                printf("Error reading .mcf file. Expected a material entry and didnt get one.\n");
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

            bool hasDiffuse  = flag & 1;
            bool hasNormal   = flag & 2;
            bool hasAMR      = flag & 4;
            bool hasEmissive = flag & 8;

            mat.baseColorTexture.data = reserveDataBlocks<unsigned char>(1);
            mat.normalTexture.data = reserveDataBlocks<unsigned char>(1);
            mat.amrTexture.data = reserveDataBlocks<unsigned char>(1);
            mat.emissiveTexture.data = reserveDataBlocks<unsigned char>(1);

            if (hasDiffuse) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.baseColorTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.baseColorTexture.data = reserveDataBlocks<unsigned char>(numBytes);
                memcpy(mat.baseColorTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasNormal) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.normalTexture.height, data, sizeof(int)); data += sizeof(int);
                
                mat.normalTexture.data = reserveDataBlocks<unsigned char>(numBytes);
                memcpy(mat.normalTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasAMR) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.amrTexture.height, data, sizeof(int)); data += sizeof(int);
                
                mat.amrTexture.data = reserveDataBlocks<unsigned char>(numBytes);
                memcpy(mat.amrTexture.data.data, data, numBytes); data += numBytes;
            }

            if (hasEmissive) {
                int numBytes;
                memcpy(&numBytes, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.nrChannels, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.width, data, sizeof(int)); data += sizeof(int);
                memcpy(&mat.emissiveTexture.height, data, sizeof(int)); data += sizeof(int);

                mat.emissiveTexture.data = reserveDataBlocks<unsigned char>(numBytes);
                memcpy(mat.emissiveTexture.data.data, data, numBytes); data += numBytes;
            }

            mat.flag = flag;
            mat.initialised = false;

            data += 2;

            if (materials.find(mat_name) == materials.end()) {
                printf("    Material Entry: [%s] flag: %d...", mat_name, (int)flag);
                //initializeMaterial(&mat);
                printf("Initialized\n");
                materials[mat_name] = mat;
                bool done = true;
            }
        }

        bool end = true;
    }
    else {
        printf("     Incorrect file signature found...\n");
    }

    free(buffer);
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

void ResourceManager::genTextureFromData(Material_Texture* tex) {
    GLuint texID;

    //Generate OpenGL Texture
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    // set the texture wrapping/filtering options

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        texID = 0;
    }

    tex->glTexID = texID;
}

void ResourceManager::initializeMaterial(Material* mat) {
    // TODO: create openGL textures based on the data in the Material
    if (mat->flag & 1) {
        genTextureFromData(&mat->baseColorTexture);
    }
    if (mat->flag & 2) {
        genTextureFromData(&mat->normalTexture);
    }
    if (mat->flag & 4) {
        genTextureFromData(&mat->amrTexture);
    }
    if (mat->flag & 8) {
        genTextureFromData(&mat->emissiveTexture);
    }
}

void ResourceManager::setRootDirectory(char* exeLoc) {
    m_FileSystem.setRootDirectory(exeLoc);
}

meshRef ResourceManager::getMesh(std::string id) {
    if (meshes.find(id) == meshes.end()) {
        return nullptr;
    }
    else {
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

/*
void ResourceManager::initializeTriangleMesh(TriangleMesh* mesh) {
    // Perform openGL initialization of mesh
    
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
*/

/*
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
*/