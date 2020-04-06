#include "ResourceManager.hpp"

//ResourceManager g_ResourceManager;

ResourceManager::ResourceManager() :
    m_pool(4 * MEGABYTE)
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
    std::vector<std::string> allFiles = m_FileSystem.getAllFilesOnPath("resources");
    std::vector<std::string> resourceFiles;
    for (auto file : allFiles) {
        // check if file is a .mcf
        auto k = file.find_last_of('.');
        if (k != std::string::npos) {
            auto ending = file.data() + k;
            if (strcmp(ending, ".mcf") == 0) {
                // this is a mesh catalog file.
                resourceFiles.push_back(file);

                loadResourceFile(file);
            }
        }
    }

    return this;
}

struct mesh_entry {

};

struct mat_entry {

};

void ResourceManager::loadResourceFile(std::string filename) {
    printf("Reading resource file [%s]...\n", filename.c_str());
    size_t bytesRead;
    char* buffer = m_FileSystem.readAllBytes("resources\\" + filename, bytesRead);

    if (buffer && bytesRead > 0) {
        printf("Succesfully read %zu bytes\n", bytesRead);
    }
    else {
        return;
    }

    char correctSig[10] = { 'm', 'c', 'f', 72, 95, 'k', 'j', 0, '\r', '\n' };

    if (strncmp(buffer, correctSig, 10) == 0) {
        printf("Correct file signature found. Reading...\n");

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

            char* dump = data += 6;
            size_t end = 0;
            while (data[end] != 0 && data[end] != 10 && data[end] != 13) {
                end++;
            }

            char* mesh_name = (char*)malloc(end+1);
            memcpy(mesh_name, data, end);
            mesh_name[end] = 0;
            printf("Mesh Entry: [%s]\n", mesh_name);

            data += end+2;

            char flag = data[0];
            data++;

            int numIndices;
            memcpy(&numIndices, data, 4);
            data += 4;
            int* mesh_indices = (int*)malloc(numIndices * sizeof(int));
            memcpy(mesh_indices, data, numIndices * sizeof(int));
            data += numIndices * sizeof(int);

            int numVerts;
            memcpy(&numVerts, data, 4);
            data += 4;
            float* mesh_verts = (float*)malloc(numVerts * 3 * sizeof(float));
            memcpy(mesh_verts, data, numVerts * 3 * sizeof(float));
            data += numVerts * 3 * sizeof(float);

            bool hasNormals = flag & 1;
            bool hasUVs = flag & 2;
            bool hasTangents = flag & 4;

            if (hasNormals) {
                float* mesh_normals = (float*)malloc(numVerts * 3 * sizeof(float));
                memcpy(mesh_normals, data, numVerts * 3 * sizeof(float));
                data += numVerts * 3 * sizeof(float);

                free(mesh_normals);
            }

            if (hasUVs) {
                float* mesh_UVs = (float*)malloc(numVerts * 2 * sizeof(float));
                memcpy(mesh_UVs, data, numVerts * 2 * sizeof(float));
                data += numVerts * 2 * sizeof(float);

                free(mesh_UVs);
            }

            if (hasTangents) {
                float* mesh_tangents = (float*)malloc(numVerts * 4 * sizeof(float));
                memcpy(mesh_tangents, data, numVerts * 4 * sizeof(float));
                data += numVerts * 4 * sizeof(float);

                free(mesh_tangents);
            }

            data += 2;


            free(mesh_name);
            free(mesh_indices);
            free(mesh_verts);
        }

        bool end = true;

        /*
        char dump1[84];
        fread(dump1, 1, 84, fp);
        char txt_numMeshes[10];
        fread(txt_numMeshes, 1, 10, fp);
        char dump2[23];
        fread(dump2, 1, 23, fp);
        char txt_numMaterials[10];
        fread(txt_numMaterials, 1, 10, fp);
        char dump3[22];
        fread(dump3, 1, 22, fp);
        char txt_numEntities[10];
        fread(txt_numEntities, 1, 10, fp);
        char dump4[15];
        fread(dump3, 1, 15, fp);

        int numMeshes, numMaterials, numEntities;
        sscanf(txt_numMeshes, "%d", &numMeshes);
        sscanf(txt_numMaterials, "%d", &numMaterials);
        sscanf(txt_numEntities, "%d", &numEntities);
        printf("Reading meshes...\n");

        if (numMeshes > 0) {
            printf("Catalog contains %d meshes, consisting of %d total verts and %d total indices.\n", numMeshes, numMaterials, numEntities);

            for (int n = 0; n < numMeshes; n++) {
                char dump5[7];
                fread(dump5, 1, 6, fp);
                dump5[6] = 0;

                if (strcmp(dump5, "Mesh: ") == 0) {
                    printf("Mesh found.\n");

                    char name[64] = { 0 };
                    fscanf(fp, "%s", name);

                    printf("Name: [%s]\n", name);

                    char dump6[2];
                    fread(dump6, 1, 2, fp);

                    char flag;
                    fread(&flag, 1, 1, fp);
                    printf("Flag: %d\n", flag);

                    int numIndices;
                    fread(&numIndices, 1, sizeof(int), fp);
                    printf("%d indices\n", numIndices);

                    int* indices = (int*)malloc(numIndices * sizeof(int));
                    fread(indices, sizeof(int), numIndices, fp);

                    int numVerts;
                    fread(&numVerts, 1, sizeof(int), fp);
                    printf("%d verts\n", numVerts);

                    float* verts = (float*)malloc(numVerts * 3 * sizeof(float));
                    fread(verts, sizeof(float), numVerts * 3, fp);


                    if (flag & 1) {
                        printf("Has Normals\n");

                        float* normals = (float*)malloc(numVerts * 3 * sizeof(float));
                        fread(normals, sizeof(float), numVerts * 3, fp);

                        free(normals);
                    }
                    if (flag & 2) {
                        printf("Has UVs\n");

                        float* uvs = (float*)malloc(numVerts * 2 * sizeof(float));
                        fread(uvs, sizeof(float), numVerts * 2, fp);

                        free(uvs);
                    }
                    if (flag & 4) {
                        printf("Has Tangents\n");

                        float* tangents = (float*)malloc(numVerts * 4 * sizeof(float));
                        fread(tangents, sizeof(float), numVerts * 4, fp);

                        free(tangents);
                    }


                    free(verts);
                    free(indices);
                }

                char dump7[2];
                fread(dump7, 1, 2, fp);
            }
        }
        else {
            printf("Could not find any meshes in the file\n");
        }*/

    }

    free(buffer);
}

void ResourceManager::setRootDirectory(char* exeLoc) {
    m_FileSystem.setRootDirectory(exeLoc);
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