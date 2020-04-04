#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

using namespace tinygltf;
using namespace std;

struct MeshCatalog {
    FILE* fp;           /* File pointer */

    long meshNumOffset; /* byte offsets inside file */
    long vertNumOffset;
    long indexNumOffset;

    int numMeshes;      /* catalog totals */
    int numMaterials;
    int numEntities;
};

MeshCatalog createCatalog(string catalogFilename);
void addToCatalog(MeshCatalog* catalog, string gltfFilename);
void updateCatalogHeader(MeshCatalog catalog);

void readMeshCatalogFile(string catalogFilename);

int main(int argc, char** argv) {
    //system("pause");
    printf("\n-------------------\n");
    printf("Mesh Converter v1.0\n");
    printf("-------------------\n\n");

    if (argc <= 1) {
        printf("Rerun meshConvert pointing to glTF files you wish to convert.\n");
        printf("Example:\n\n meshConvert input1.gltf input2.glb output.mcf\n\n");
        return 0;
    }

    string outputFilename;
    vector<string> inputFilenames;

    if (argc == 2) {
        outputFilename = "out.mcf";
        inputFilenames.push_back(string(argv[1]));
    }
    else {
        for (int n = 1; n < argc - 1; n++) {
            inputFilenames.push_back(string(argv[n]));
        }
        outputFilename = string(argv[argc-1]);
    }

    printf("Output file:\n  %s\n", outputFilename.c_str());
    printf("Input files: %d\n", inputFilenames.size());
    for (auto str : inputFilenames) {
        printf("  %s\n", str.c_str());
    }
    printf("-------------------\n\n");

    /* Create mesh catalog */
    MeshCatalog catalog = createCatalog(outputFilename);

    /* Add meshes to catalog */
    for (auto str : inputFilenames) {
        addToCatalog(&catalog, str);
    }

    updateCatalogHeader(catalog);

    fclose(catalog.fp);

    // now read the file
    readMeshCatalogFile(outputFilename);

    system("pause");
    return 0;
}

void readMeshCatalogFile(string catalogFilename) {
    FILE* fp = fopen(catalogFilename.c_str(), "rb");

    char correctSig[10] = { 'm', 'c', 'f', 72, 95, 'k', 'j', 0, '\r', '\n' };
    char filesig[10];
    fread(filesig, 1, 10, fp);

    if (strcmp(filesig, correctSig) == 0) {
        printf("Correct file signature found. Reading...\n");

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
        }

    }

    fclose(fp);
}

/* Create a new MeshCatalog file and write its header */
MeshCatalog createCatalog(string catalogFilename) {
    /* Create file */
    FILE* catalog = fopen(catalogFilename.c_str(), "wb");

    /* Write file header */
    char header[50];
    memset(header, static_cast<int>('\0'), sizeof(header));

    // write file signature
    char filesig[10] = {'m', 'c', 'f', 72, 95, 'k', 'j', 0, '\r', '\n'};
    char* ptr = header;
    memcpy(ptr, filesig, sizeof(filesig));

    // format version & Date of Creation
    ptr += sizeof(filesig);
    sprintf(ptr, "Made with MeshConvert 1.0\r\nCreated on: ");

    // Write timestamp
    time_t ltime = time(NULL);
    char* t = asctime(localtime(&ltime));
    char buf[26];
    memcpy(buf, t, sizeof(buf));

    buf[24] = 13;
    buf[25] = 10;

    // Write buffers to file
    fwrite(header, sizeof(header), 1, catalog);
    fwrite(buf, sizeof(buf), 1, catalog);

    /* Store location of mesh, vert, and index file pointers */
    long meshNumberLoc, vertNumberLoc, indexNumberLoc;
    const int placeholderWidth = 10;
    char placeholder[placeholderWidth+1];
    memset(placeholder, ' ', sizeof(placeholder));
    placeholder[10] = 0;

    fprintf(catalog, "Number of meshes: ");
    meshNumberLoc = ftell(catalog);             // store the offset
    fprintf(catalog, "%s", placeholder);        // write placeholder characters: MAX 6 CHARS

    fprintf(catalog, "\r\nNumber of materials: ");  // repeat for other offsets +4
    vertNumberLoc = ftell(catalog);
    fprintf(catalog, "%s", placeholder);

    fprintf(catalog, "\r\nNumber of entities: "); // +1
    indexNumberLoc = ftell(catalog);
    fprintf(catalog, "%s", placeholder);

    fprintf(catalog, "\r\nEndOfHeader\r\n");

    /* Create meshCatalog return struct */
    MeshCatalog mc;
    mc.fp = catalog;
    mc.indexNumOffset = indexNumberLoc;
    mc.meshNumOffset = meshNumberLoc;
    mc.vertNumOffset = vertNumberLoc;

    return mc;
}

/*struct vec2 {
    float x, y;
};
struct vec3 {
    float x, y, z;
};
struct vec4 {
    float x, y, z, w;
};*/

struct MeshEntry {
    std::string name;

    bool hasNormals, hasUVs, hasTangents;

    int numVerts;
    int numIndices;

    float* locationsArr; //3
    float* normalsArr;   //3
    float* tangentsArr;  //4
    float* uvsArr;       //2
    int* indsArr;        //1
};

struct tex_info {
    int width, height, nrComponents;
    int bytelength;
};

struct texture_header {
    tex_info info;
    unsigned char* tex_data;
};

struct MatEntry {
    std::string name;

    bool hasDiffuseTexture, hasNormalTexture, hasAMRTexture;

    float diffuseColor[4];
    float metallicFactor, roughnessFactor;

    texture_header normalTexture;
    texture_header diffuseTexture;
    texture_header amrTexture;
};

/* This allocates memory. Need to free it later */
int* readAccessor_int(tinygltf::Accessor* acc, tinygltf::Model* model, int* out_numElements = nullptr) {
    acc->componentType;
    acc->count;
    acc->type;
    acc->bufferView;

    auto bufferView = model->bufferViews[acc->bufferView];
    bufferView.buffer;
    bufferView.byteLength;
    bufferView.byteOffset;

    int typesize = GetComponentSizeInBytes(acc->componentType);
    int numComponents = GetNumComponentsInType(acc->type);
    int numElements = acc->count * numComponents;

    if (out_numElements != nullptr)
        *out_numElements = numElements;

    auto buffer = model->buffers[bufferView.buffer];
    auto data_ptr = buffer.data.data();
    unsigned char* data = (unsigned char*)malloc(bufferView.byteLength);
    memcpy(data, data_ptr + bufferView.byteOffset, bufferView.byteLength);

    int* out = (int*)malloc(numElements * sizeof(int));

    for (int n = 0; n < numElements; n++) {
        out[n] = static_cast<int>((data[n*typesize]));
    }

    free(data);

    return out;
}

/* This allocates memory. Need to free it later */
float* readAccessor_float(tinygltf::Accessor* acc, tinygltf::Model* model, int* out_numElements = nullptr) {
    acc->componentType; // c data type (short, int, long, float, etc...)
    acc->count;
    acc->type; // real data type (scalar, vec2, vec3, vec4, etc...)
    acc->bufferView;

    int typesize = GetComponentSizeInBytes(acc->componentType);
    int numComponents = GetNumComponentsInType(acc->type);
    int numElements = acc->count * numComponents;

    if (out_numElements != nullptr)
        *out_numElements = numElements;

    auto bufferView = model->bufferViews[acc->bufferView];
    bufferView.buffer;
    bufferView.byteLength;
    bufferView.byteOffset;

    auto buffer = model->buffers[bufferView.buffer];
    auto data_ptr = buffer.data.data();
    unsigned char* data = (unsigned char*)malloc(bufferView.byteLength);
    memcpy(data, data_ptr + bufferView.byteOffset, bufferView.byteLength);

    float* out = (float*)malloc(numElements * sizeof(float));

    for (int n = 0; n < numElements; n++) {
        out[n] = static_cast<float>((data[n*typesize]));
    }

    free(data);

    return out;
}

MeshEntry processMesh(tinygltf::Mesh* mesh, tinygltf::Model* model) {
    MeshEntry meshEntry;

    printf("Mesh name: %s\n", mesh->name.c_str());
    meshEntry.name = mesh->name;
    meshEntry.hasNormals = false;
    meshEntry.hasTangents = false;
    meshEntry.hasUVs = false;

    meshEntry.indsArr = nullptr;
    meshEntry.locationsArr = nullptr;
    meshEntry.uvsArr = nullptr;
    meshEntry.normalsArr = nullptr;
    meshEntry.tangentsArr = nullptr;

    int num = 0;
    for (auto p : mesh->primitives) {
        printf("Primitive #%d\n", num);
        num++;
        if (num == 2) {
            printf("CANNOT HANDLE MULTI-PRIMITIVE MESHES YET\N");
            printf("Skipping rest of primitives.\n");
            break;
        }

        auto indices_accessor = model->accessors[p.indices];
        meshEntry.indsArr = readAccessor_int(&indices_accessor, model, &meshEntry.numIndices);

        printf("Indices: %d\n", p.indices);
        printf(" %d Indices\n", meshEntry.numIndices);

        printf("Attributes: \n");
        for (auto att : p.attributes) {
            printf("%s: %d\n", att.first.c_str(), att.second);

            auto attribute_accessor = model->accessors[att.second];
            int count;
            float* prim_data = readAccessor_float(&attribute_accessor, model, &count);

            if (att.first.compare("POSITION") == 0) {
                meshEntry.locationsArr = prim_data;
                meshEntry.numVerts = attribute_accessor.count; // want to know how many vec-3s there are, which is already accounted for
                printf(" %d Verts\n", attribute_accessor.count);
            }
            else if (att.first.compare("NORMAL") == 0) {
                meshEntry.hasNormals = true;
                meshEntry.normalsArr = prim_data;
                printf(" %d Normals\n", attribute_accessor.count);
            }
            else if (att.first.compare("TANGENT") == 0) {
                meshEntry.hasTangents = true;
                meshEntry.tangentsArr = prim_data;
                printf(" %d Tangents\n", attribute_accessor.count);
            }
            else if (att.first.compare("TEXCOORD_0") == 0) {
                meshEntry.hasUVs = true;
                meshEntry.uvsArr = prim_data;
                printf(" %d UVs\n", attribute_accessor.count);
            }
        }

        printf("Primitive material: %d\n", p.material);
    }
    printf("\n");

    return meshEntry;
}

void writeMeshEntry(MeshCatalog* catalog, MeshEntry meshEntry) {
    // Write mesh data to file.
    catalog->numMeshes++;

    unsigned char flag = 0;
    if (meshEntry.hasNormals) {
        flag += 1;
    }
    if (meshEntry.hasUVs) {
        flag += 2;
    }
    if (meshEntry.hasTangents) {
        flag += 4;
    }

    fprintf(catalog->fp, "Mesh: %s\r\n", meshEntry.name.c_str());

    fwrite(&flag, sizeof(flag), 1, catalog->fp);

    fwrite(&meshEntry.numIndices, sizeof(int), 1, catalog->fp);
    fwrite(meshEntry.indsArr, sizeof(int), meshEntry.numIndices, catalog->fp);

    fwrite(&meshEntry.numVerts, sizeof(int), 1, catalog->fp);
    fwrite(meshEntry.locationsArr, sizeof(float), meshEntry.numVerts * 3, catalog->fp);

    if (meshEntry.hasNormals) {
        fwrite(meshEntry.normalsArr, sizeof(float), meshEntry.numVerts * 3, catalog->fp);
    }
    if (meshEntry.hasUVs) {
        fwrite(meshEntry.uvsArr, sizeof(float), meshEntry.numVerts * 2, catalog->fp);
    }
    if (meshEntry.hasTangents) {
        fwrite(meshEntry.tangentsArr, sizeof(float), meshEntry.numVerts * 4, catalog->fp);
    }

    fprintf(catalog->fp, "\r\n");

    // temp delete for cleanup
    if (meshEntry.locationsArr)
        free(meshEntry.locationsArr);
    if (meshEntry.indsArr)
        free(meshEntry.indsArr);
    if (meshEntry.normalsArr)
        free(meshEntry.normalsArr);
    if (meshEntry.uvsArr)
        free(meshEntry.uvsArr);
    if (meshEntry.tangentsArr)
        free(meshEntry.tangentsArr);
}

texture_header readTextureData(int index, tinygltf::Model* model) {
    auto texture = model->textures[index];
    auto image = model->images[texture.source];
    auto bufferView = model->bufferViews[image.bufferView];
    auto buffer = model->buffers[bufferView.buffer];

    unsigned char* data = (unsigned char*)malloc(bufferView.byteLength);
    memcpy(data, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);

    texture_header out;
    out.info.height = image.height;
    out.info.width = image.width;
    out.info.nrComponents = image.component;
    out.info.bytelength = bufferView.byteLength;
    //int bitSize = GetComponentSizeInBytes(image.pixel_type) * 8;
    //int totalBits = image.bits;
    out.tex_data = data;

    return out;
}

MatEntry processMat(tinygltf::Material* mat, tinygltf::Model* model) {
    // Initialize struct
    MatEntry matEntry;
    matEntry.name = mat->name;

    matEntry.hasDiffuseTexture = mat->pbrMetallicRoughness.baseColorTexture.index >= 0;
    matEntry.hasNormalTexture = mat->normalTexture.index >= 0;
    matEntry.hasAMRTexture = mat->pbrMetallicRoughness.metallicRoughnessTexture.index >= 0 && 
                             mat->occlusionTexture.index >= 0 && 
                             (mat->pbrMetallicRoughness.metallicRoughnessTexture.index == mat->occlusionTexture.index);

    matEntry.diffuseColor[0] = mat->pbrMetallicRoughness.baseColorFactor[0];
    matEntry.diffuseColor[1] = mat->pbrMetallicRoughness.baseColorFactor[1];
    matEntry.diffuseColor[2] = mat->pbrMetallicRoughness.baseColorFactor[2];
    matEntry.diffuseColor[3] = mat->pbrMetallicRoughness.baseColorFactor[3];

    matEntry.roughnessFactor = mat->pbrMetallicRoughness.roughnessFactor;
    matEntry.metallicFactor = mat->pbrMetallicRoughness.metallicFactor;

    // read diffuse texture
    if (matEntry.hasDiffuseTexture)
        matEntry.diffuseTexture = readTextureData(mat->pbrMetallicRoughness.baseColorTexture.index, model);
    else
        matEntry.diffuseTexture.tex_data = nullptr;

    // read normal texture
    if (matEntry.hasNormalTexture)
        matEntry.normalTexture = readTextureData(mat->normalTexture.index, model);
    else
        matEntry.normalTexture.tex_data = nullptr;

    // read ambient, metallic, roughness texture
    if (matEntry.hasAMRTexture)
        matEntry.amrTexture = readTextureData(mat->occlusionTexture.index, model);
    else
        matEntry.amrTexture.tex_data = nullptr;

    // Read data from tinygltf material
    printf("Material name: %s\n", mat->name.c_str());
    if (matEntry.hasDiffuseTexture)
        printf("  has diffuse texture\n");
    if (matEntry.hasNormalTexture)
        printf("  has normal texture\n");
    if (matEntry.hasAMRTexture)
        printf("  has AMR texture\n");

    return matEntry;
}

void writeMatEntry(MeshCatalog* catalog, MatEntry matEntry) {
    // Write mat data to file.
    catalog->numMaterials++;

    unsigned char flag = 0;
    if (matEntry.hasDiffuseTexture) {
        flag += 1;
    }
    if (matEntry.hasNormalTexture) {
        flag += 2;
    }
    if (matEntry.hasAMRTexture) {
        flag += 4;
    }

    fprintf(catalog->fp, "Material: %s\r\n", matEntry.name.c_str());

    fwrite(&flag, sizeof(flag), 1, catalog->fp);
    fwrite(matEntry.diffuseColor, sizeof(float), 4, catalog->fp); // 4 * 4 = 16 bytes
    fwrite(&matEntry.metallicFactor, sizeof(float), 1, catalog->fp); // 1 * 4 = 4 bytes
    fwrite(&matEntry.roughnessFactor, sizeof(float), 1, catalog->fp); // 1 * 4 = 4 bytes

    if (matEntry.hasDiffuseTexture) {
        size_t numWritten = fwrite(&matEntry.diffuseTexture.info.bytelength, 1, sizeof(int), catalog->fp);
        numWritten = fwrite(matEntry.diffuseTexture.tex_data, 1, matEntry.diffuseTexture.info.bytelength, catalog->fp);
    }

    if (matEntry.hasNormalTexture) {
        fwrite(&matEntry.normalTexture.info.bytelength, 1, sizeof(int), catalog->fp);
        fwrite(matEntry.normalTexture.tex_data, 1, matEntry.normalTexture.info.bytelength, catalog->fp);
    }

    if (matEntry.hasAMRTexture) {
        fwrite(&matEntry.amrTexture.info.bytelength, 1, sizeof(int), catalog->fp);
        fwrite(matEntry.amrTexture.tex_data, 1, matEntry.amrTexture.info.bytelength, catalog->fp);
    }

    if (matEntry.diffuseTexture.tex_data)
        free(matEntry.diffuseTexture.tex_data);
    if (matEntry.normalTexture.tex_data)
        free(matEntry.normalTexture.tex_data);
    if (matEntry.amrTexture.tex_data)
        free(matEntry.amrTexture.tex_data);

    fprintf(catalog->fp, "\r\n");
}

/* Load glTF file and add all its meshes to the catalog */
void addToCatalog(MeshCatalog* catalog, string gltfFilename) {
    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    /* Determine filetype from filename */
    bool binary = false;
    auto k = gltfFilename.find_last_of('.');
    auto t = gltfFilename.substr(k+1, 3);
    if (t.compare("glb") == 0) {
        printf(".glb found\n");
        binary = true;
    }
    else if (t.compare("glt") == 0) {
        printf(".gltf found\n");
        binary = false;
    }
    else {
        printf("Invalid filetype\n");
        return;
    }

    bool ret = false;
    if (binary) {
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, gltfFilename);
    }
    else {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, gltfFilename);
    }

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
        return;
    }

    // add mesh data to catalog.
    bool stop = true;
    catalog->numMeshes = 0;
    catalog->numMaterials = 0;
    catalog->numEntities = 0;

    printf("\n\n");
    /* Read and write meshes to catalog */
    for (auto mesh : model.meshes) {
        // Read entry from file
        MeshEntry meshEntry = processMesh(&mesh, &model);
        // Write mesh entry to file.
        writeMeshEntry(catalog, meshEntry);
    }

    /* Read and write materials to catalog */
    for (auto mat : model.materials) {
        // read material from file
        MatEntry matEntry = processMat(&mat, &model);
        writeMatEntry(catalog, matEntry);
        // write material entry to catalog
    }

    printf("# of meshes: %d\n", catalog->numMeshes);
    printf("# of materials: %d\n", catalog->numMaterials);
    printf("# of entities: %d\n", catalog->numEntities);
}

/* Update the header at the start of the file to show how many meshes, verts, and indices are stored. */
void updateCatalogHeader(MeshCatalog catalog) {
    long saveLoc = ftell(catalog.fp); // store current location for later.

    // write in reverse order, in case the byte offsets get changed.

    // write number of indices
    fseek(catalog.fp, catalog.indexNumOffset, SEEK_SET);
    fprintf(catalog.fp, "%d", catalog.numEntities);
    // write number of verts
    fseek(catalog.fp, catalog.vertNumOffset, SEEK_SET);
    fprintf(catalog.fp, "%d", catalog.numMaterials);
    // write number of meshes
    fseek(catalog.fp, catalog.meshNumOffset, SEEK_SET);
    fprintf(catalog.fp, "%d", catalog.numMeshes);

    fseek(catalog.fp, saveLoc, SEEK_SET); // return to save point
}