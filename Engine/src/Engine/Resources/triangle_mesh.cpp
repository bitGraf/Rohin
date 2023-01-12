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
struct mesh_file_joint {
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

void LoadMeshFromBuffer(memory_arena* Arena, triangle_mesh* Mesh, uint8* Buffer, uint32 BufferSize) {
    /* TODO:
     * Rewrite mesh file layout to minimize the individual buffer copies we need to do.
     * Ideally, we read in the header and with the values in the header we can 
     * pre-"allocate" from the arena and then just do one memcopy into the actual
     * buffers we need.
     * 
     * Reconsider size of integer types in the mesh file format.
     * We do a lot of:
     *    -read uint16 from the file
     *    -convert it to uint8 in the triangle_mesh since we dont expect that high of a number
     * Would be better for data-packing reasons as well for the file.
     * 
     * Mesh file currently does not support materials!
     * */
    uint8* End = Buffer + BufferSize;
    mesh_file_header* Header = AdvanceBuffer(&Buffer, mesh_file_header, End);

    if (!CheckHeader(Header, BufferSize)) {
        Assert(!"Incorrect header\n");
        return;
    }
    AdvanceBufferSize_(&Buffer, Header->CommentLength, End);

    // decode mesh flags
    bool32 HasSkeleton = Header->Flag & 0x01;

    Mesh->NumSubmeshes = (uint8)Header->NumSubmeshes;
    Assert(Mesh->NumSubmeshes == Header->NumSubmeshes); // 16->8 bits conversion
    Mesh->Submeshes = PushArray(Arena, submesh, Mesh->NumSubmeshes);
    for (int SubmeshIndex = 0; SubmeshIndex < Header->NumSubmeshes; SubmeshIndex++) {
        mesh_file_submesh* BufferSubmesh = AdvanceBuffer(&Buffer, mesh_file_submesh, End);
        submesh* Submesh = &Mesh->Submeshes[SubmeshIndex];
        Submesh->BaseVertex = 0;
        Submesh->BaseIndex = BufferSubmesh->BaseIndex;
        Submesh->MaterialIndex = BufferSubmesh->MaterialIndex; // NOTE: mesh file will always have 0 here...
        Submesh->IndexCount = BufferSubmesh->IndexCount;

        Submesh->Transform = rh::laml::Mat4(BufferSubmesh->Transform);
    }

    // TODO: Add proper material support to the file format
    Mesh->NumMaterials = 1;
    Assert(Mesh->NumMaterials == 1);
    Mesh->Materials = PushArray(Arena, material, Mesh->NumMaterials);
    for (int MaterialIndex = 0; MaterialIndex < 1; MaterialIndex++) {
        material* Material = &Mesh->Materials[MaterialIndex];

        // this can't work at the moment.
        // TODO: time restructure a lot of things now...
        //Win32LoadTextureFromFile(&Material->Diffuse, "Data/Textures/frog.png");
    }

    if (HasSkeleton) {
        AdvanceBufferSize_(&Buffer, 4, End);
        uint16* NumJoints = AdvanceBuffer(&Buffer, uint16, End);
        Mesh->NumJoints = (uint8)(*NumJoints);
        Assert(Mesh->NumJoints == (*NumJoints)); // 16->8 bits conversion
        Mesh->Joints = PushArray(Arena, joint, Mesh->NumJoints);
        Mesh->JointsDebug = PushArray(Arena, joint_debug, Mesh->NumJoints);

        for (uint16 JointIndex = 0; JointIndex < *NumJoints; JointIndex++) {
            uint16* JointNameLength = AdvanceBuffer(&Buffer, uint16, End);
            char* JointName = (char*)AdvanceBufferSize_(&Buffer, *JointNameLength, End);

            mesh_file_joint* BufferJoint = AdvanceBuffer(&Buffer, mesh_file_joint, End);

            joint* Joint = &Mesh->Joints[JointIndex];
            Joint->ParentIndex = BufferJoint->ParentIndex;
            Joint->LocalMatrix = BufferJoint->LocalMatrix;
            Joint->InverseModelMatrix = BufferJoint->InverseModelMatrix;
            rh::laml::identity(Joint->FinalTransform);

            joint_debug* JointDebug = &Mesh->JointsDebug[JointIndex];
            JointDebug->Length = BufferJoint->DebugLength;
            JointDebug->Name = PushArray(Arena, char, *JointNameLength);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < *JointNameLength; nn++) {
                JointDebug->Name[nn] = JointName[nn];
            }
            JointDebug->ModelMatrix = rh::laml::inverse(Joint->InverseModelMatrix); // slow!
        }
    }

    // TODO: Do we need to cache the vertices/indices of the mesh? 
    // or just send them to the GPU and discard.
    // Easier to just discard, but we might need them.
    // Only use case I can think of is for the collision geometry, so maybe
    // we pass it to there first then discard it.
    AdvanceBufferArray(&Buffer, char, 4, End); // DATA
    AdvanceBufferArray(&Buffer, char, 4, End); // IDX\0

    uint32* Indices = AdvanceBufferArray(&Buffer, uint32, Header->NumInds, End);

    AdvanceBufferArray(&Buffer, char, 4, End); // VERT

    size_t VertexDataSize = HasSkeleton ? Header->NumVerts * sizeof(vertex_anim) : Header->NumVerts * sizeof(vertex_static) ;
    void* VertexData = AdvanceBufferSize_(&Buffer, (uint32)VertexDataSize, End);

    if (HasSkeleton) {
        AdvanceBufferArray(&Buffer, char, 4, End); // ANIM

        uint16* NumAnimsInCatalog = AdvanceBuffer(&Buffer, uint16, End);
        Mesh->NumAnimations = (uint8)(*NumAnimsInCatalog);
        Assert(Mesh->NumAnimations == *NumAnimsInCatalog);
        Mesh->Animations = PushArray(Arena, animation, Mesh->NumAnimations);
        Mesh->AnimationsDebug = PushArray(Arena, animation_debug, Mesh->NumAnimations);

        for (uint16 Animindex = 0; Animindex < *NumAnimsInCatalog; Animindex++) {
            uint16* AnimNameLen = AdvanceBuffer(&Buffer, uint16, End);
            char* AnimName = AdvanceBufferArray(&Buffer, char, *AnimNameLen, End);

            // No actual animation data is stored here, just the names to look up later...
            //animation* Animation = &Mesh->Animations[Animindex];

            animation_debug* AnimationDebug = &Mesh->AnimationsDebug[Animindex];
            AnimationDebug->Name = PushArray(Arena, char, *AnimNameLen);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < *AnimNameLen; nn++) {
                AnimationDebug->Name[nn] = AnimName[nn];
            }
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