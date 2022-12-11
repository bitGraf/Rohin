#include <enpch.hpp>
#include "Engine/Collision/CollisionHull.hpp"

namespace rh {

    CollisionHull::CollisionHull() :
        m_hullID(getNextUID()),
        m_radius(0.0f),
        rotation(1.0f)
    {}

    int CollisionHull::GetSupport(laml::Vec3 search_dir) {
        /* This ALL happens inside model space */
        laml::Scalar maxDist = laml::dot(vertices[0], search_dir);
        int maxIndex = 0;
        for (int n = 0; n < vertices.size(); n++) {
            auto v = &vertices[n];
            laml::Scalar dist = laml::dot(*v, search_dir);

            if (dist > maxDist) {
                maxDist = dist;
                maxIndex = n;
            }
        }

        assert(maxIndex >= 0);
        return maxIndex;
    }

    laml::Vec3 CollisionHull::GetVertWorldSpace(int index) {
        laml::Vec3 local = vertices[index];
        return laml::transform::transform_point(rotation, local) + position;
    }

    void CollisionHull::bufferData() {
        struct _vertex
        {
            laml::Vec3 Position;
        };

        _vertex* data = reinterpret_cast<_vertex*>(vertices.data());
        u16* _indices = reinterpret_cast<u16*>(faces.data());
        u32* indices = (u32*)malloc(faces.size() * 3 * sizeof(u32));
        for (int n = 0; n < faces.size() * 3; n++) {
            indices[n] = _indices[n];
        }

        auto vbo = rh::VertexBuffer::Create(data, vertices.size() * sizeof(_vertex));
        vbo->SetLayout({
            { rh::ShaderDataType::Float3, "a_Position" }
            });
        auto ebo = rh::IndexBuffer::Create(indices, faces.size() * 3);

        wireframe = rh::VertexArray::Create();
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

}