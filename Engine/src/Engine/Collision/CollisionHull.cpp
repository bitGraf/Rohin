#include "enpch.hpp"
#include "CollisionHull.hpp"

CollisionHull::CollisionHull() :
    m_hullID(getNextUID()),
    m_radius(0.0f)
{}

int CollisionHull::GetSupport(vec3 search_dir) {
    /* This ALL happens inside model space */
    scalar maxDist = vertices[0].dot(search_dir);
    int maxIndex = 0;
    for (int n = 0; n < vertices.size(); n++) {
        auto v = &vertices[n];
        scalar dist = v->dot(search_dir);

        if (dist > maxDist) {
            maxDist = dist;
            maxIndex = n;
        }
    }

    assert(maxIndex >= 0);
    return maxIndex;
}

vec3 CollisionHull::GetVertWorldSpace(int index) {
    vec3 local = vertices[index];
    return rotation * local + position;
}

void CollisionHull::bufferData() {
    struct _vertex
    {
        math::vec3 Position;
    };

    _vertex* data = reinterpret_cast<_vertex*>(vertices.data());
    index_t* _indices = reinterpret_cast<index_t*>(faces.data());
    u32* indices = (u32*)malloc(faces.size() * 3 * sizeof(u32));
    for (int n = 0; n < faces.size() * 3; n++) {
        indices[n] = _indices[n];
    }

    auto vbo = Engine::VertexBuffer::Create(data, vertices.size() * sizeof(_vertex));
    vbo->SetLayout({
        { Engine::ShaderDataType::Float3, "a_Position" }
        });
    auto ebo = Engine::IndexBuffer::Create(indices, faces.size() * 3);

    wireframe = Engine::VertexArray::Create();
    wireframe->Bind();
    wireframe->AddVertexBuffer(vbo);
    wireframe->SetIndexBuffer(ebo);
    wireframe->Unbind();

    /*
    glGenVertexArrays(1, &wireframeVAO);
    glBindVertexArray(wireframeVAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.blockBytes, vertices.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices.m_elementSize, (void*)0);
    glEnableVertexAttribArray(0);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.blockBytes, faces.data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    */
}


UID_t CollisionHull::nextUID = 0;
UID_t CollisionHull::getNextUID() {
    return ++nextUID; // starts at 1
}
