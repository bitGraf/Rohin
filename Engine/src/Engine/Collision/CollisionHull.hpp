#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/VertexArray.hpp"

namespace rh {
    typedef u64 UID_t;

    struct CollisionTriangle {
        u16 indices[3];
        CollisionTriangle() : indices{ 0,0,0 } {}
        CollisionTriangle(u16 n1, u16 n2, u16 n3) : indices{ n1,n2,n3 } {}
    };

    class CollisionHull {
    public:
        CollisionHull();

        int GetSupport(laml::Vec3 search_dir);
        laml::Vec3 GetVertWorldSpace(int index);

        laml::Vec3 position;
        laml::Mat3 rotation;
        rh::Ref<rh::VertexArray> wireframe; // for visualizing

        void bufferData();

        std::vector<laml::Vec3> vertices;
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
