#include "triangle_mesh.h"

bool32 CheckTag(uint8* Buffer, char Token[], size_t NumChars) {
    for (int n = 0; n < NumChars; n++) {
        if (*Buffer++ != Token[n]) {
            return false;
        }
    }
    return true;
}

struct mesh_file_header {
    unsigned char Magic[4];
    uint32 FileSize;
    uint32 Flag;
    unsigned char Info[4];
    uint32 NumVerts;
    uint32 NumInds;
    uint16 NumSubmeshes;
    unsigned char VersionString[4];
    uint16 CommentLength;
};
struct mesh_file_submesh {
    char Tag[8];
    uint32 BaseIndex;
    uint32 MaterialIndex;
    uint32 IndexCount;
    real32 Transform[16];
};
struct mesh_file_bone {
    int32 ParentIndex;
    real32 DebugLength;
    real32 LocalMatrix[16];
    real32 InverseModelMatrix[16];
};

bool32 CheckHeader(mesh_file_header* Header, uint32 Size) {
    if (Header->FileSize != Size) return false;
    if (!CheckTag(Header->Magic, "MESH", 4)) return false;
    if (!CheckTag(Header->Info, "INFO", 4)) return false;

    if (Header->VersionString[0] != 'v') return false;
    uint8 VersionMajor = Header->VersionString[1];
    uint8 VersionMinor = Header->VersionString[2];
    uint8 VersionPatch = Header->VersionString[3];
    // check version number

    return true;
}

#define AdvanceBufferArray(Buffer, Type, Count, End) (Type*)AdvanceBufferSize_(Buffer, (Count)*sizeof(Type), End)
#define AdvanceBuffer(Buffer, Type, End) (Type*)AdvanceBufferSize_(Buffer, sizeof(Type), End)
uint8* AdvanceBufferSize_(uint8** Buffer, uint32 Size, uint8* End) {
    Assert((*Buffer + Size) <= End);
    uint8* Result = *Buffer;
    *Buffer += Size;
    return Result;
}

void LoadMeshFromBuffer(triangle_mesh* Mesh, uint8* Buffer, uint32 BufferSize) {
    uint8* End = Buffer + BufferSize;
    mesh_file_header* Header = AdvanceBuffer(&Buffer, mesh_file_header, End);

    if (!CheckHeader(Header, BufferSize)) {
        Assert(!"Incorrect header\n");
        return;
    }
    AdvanceBufferSize_(&Buffer, Header->CommentLength, End);

    bool32 HasSkeleton = Header->Flag & 0x01;

    for (int SubmeshIndex = 0; SubmeshIndex < Header->NumSubmeshes; SubmeshIndex++) {
        mesh_file_submesh* Submesh = AdvanceBuffer(&Buffer, mesh_file_submesh, End);
    }

    if (HasSkeleton) {
        AdvanceBufferSize_(&Buffer, 4, End);
        uint16* NumBones = AdvanceBuffer(&Buffer, uint16, End);

        //mesh_file_bone* Bones = AdvanceBufferArray(&Buffer, mesh_file_bone, NumBones, End);
        for (uint16 BoneIndex = 0; BoneIndex < *NumBones; BoneIndex++) {
            uint16* BoneNameLength = AdvanceBuffer(&Buffer, uint16, End);
            AdvanceBufferSize_(&Buffer, *BoneNameLength, End);

            mesh_file_bone* Bone = AdvanceBuffer(&Buffer, mesh_file_bone, End);
        }
    }

    AdvanceBufferArray(&Buffer, char, 4, End); // DATA
    AdvanceBufferArray(&Buffer, char, 4, End); // IDX\0

    uint32* Indices = AdvanceBufferArray(&Buffer, uint32, Header->NumInds, End);

    AdvanceBufferArray(&Buffer, char, 4, End); // VERT

    size_t VertexDataSize = HasSkeleton ? Header->NumVerts * sizeof(vertex_anim) : Header->NumVerts * sizeof(vertex_static) ;
    void* VertexData = AdvanceBufferSize_(&Buffer, (uint32)VertexDataSize, End);

    if (HasSkeleton) {
        AdvanceBufferArray(&Buffer, char, 4, End); // ANIM

        uint16* NumAnimsInCatalog = AdvanceBuffer(&Buffer, uint16, End);

        for (uint16 Animindex = 0; Animindex < *NumAnimsInCatalog; Animindex++) {
            uint16* AnimNameLen = AdvanceBuffer(&Buffer, uint16, End);
            char* AnimName = AdvanceBufferArray(&Buffer, char, *AnimNameLen, End);

            // create Animation entry with AnimName
            int m = 5;
        }
    }

    uint8* EndTag = AdvanceBufferArray(&Buffer, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        Assert(!"Did not end up at the correct place in the file ;~;\n");
    }
    Assert(Buffer == End);

    vertex_buffer VBO;
    if (HasSkeleton) {
        VBO = Engine.Render.CreateVertexBuffer(VertexData, (uint32)VertexDataSize, 7, 
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float2,
                                               ShaderDataType::Int4,
                                               ShaderDataType::Float4);
    } else {
        VBO = Engine.Render.CreateVertexBuffer(VertexData, (uint32)VertexDataSize, 5, 
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float3,
                                               ShaderDataType::Float2);
    }
    index_buffer IBO = Engine.Render.CreateIndexBuffer(Indices, Header->NumInds);
    Mesh->VertexArray = Engine.Render.CreateVertexArray(&VBO, &IBO);

    int done = 5;
}