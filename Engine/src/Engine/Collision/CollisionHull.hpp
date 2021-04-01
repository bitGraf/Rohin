#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"
#include "Engine/Renderer/VertexArray.hpp"

namespace Engine {
    typedef u64 UID_t;

    struct CollisionTriangle {
        index_t indices[3];
        CollisionTriangle() : indices{ 0,0,0 } {}
        CollisionTriangle(index_t n1, index_t n2, index_t n3) : indices{ n1,n2,n3 } {}
    };

    class CollisionHull {
    public:
        CollisionHull();

        int GetSupport(math::vec3 search_dir);
        math::vec3 GetVertWorldSpace(int index);

        math::vec3 position;
        math::mat3 rotation;
        Engine::Ref<Engine::VertexArray> wireframe; // for visualizing

        void bufferData();

        std::vector<math::vec3> vertices;
        std::vector<CollisionTriangle> faces;
        float m_radius;

        const UID_t m_hullID;

    private:
        /// Get the next globally unique ID
        static UID_t getNextUID();
        static UID_t nextUID;
    };
}

#endif
