#include "CollisionWorld.hpp"

CollisionWorld cWorld;

CollisionWorld::CollisionWorld() {

}

void CollisionWorld::Update(double dt) {

}

RaycastResult CollisionWorld::Raycast(vec3 start, vec3 direction, scalar distance) {
    vec3 end = (direction*distance) + start;
    return Raycast(start, end);
}

RaycastResult CollisionWorld::Raycast(vec3 start, vec3 end) {
    RaycastResult res;

    vec3 d = end - start;
    scalar closest_t = 2;

    res.start = start;
    res.end = end;
    res.t = closest_t;
    res.colliderID = 0;
    res.contactPoint = end;
    res.normal = vec3(0,0,0);

    for (int n = 0; n < m_static.size(); n++) {
        // Loop through every hull in the scene
        auto hull = &m_static[n];

        // only select the ones that close
        vec3 toHull = hull->position - start;
        if (toHull.dot(d) < 0)
            continue;

        // loop through all the triangles in the hull
        for (int m = 0; m < hull->faces.m_numElements; m++) {
            auto face = hull->faces.data[m];
            vec3 v0 = hull->GetVertWorldSpace(face.indices[0]);
            vec3 v1 = hull->GetVertWorldSpace(face.indices[1]);
            vec3 v2 = hull->GetVertWorldSpace(face.indices[2]);

            vec3 normal = (v1 - v0).cross(v2 - v0).get_unit();

            scalar t = (v0 - start).dot(normal) / (d.dot(normal));
            if (t > 1.0f || t < 0.0f) {
                // ray doesn't intersect the plane with t of [0,1]
                continue;
            }

            if (t > closest_t) {
                // there is a guaranteed intersect that is closer
                continue;
            }

            vec3 Q = start + d * t; // point on the triangle's plane

            // Compute signed triangle area.
            vec3 norm = (v1 - v0).cross(v2 - v0);
            vec3 n0 = (v1 - Q).cross(v2 - Q);
            vec3 n1 = (v2 - Q).cross(v0 - Q);
            vec3 n2 = (v0 - Q).cross(v1 - Q);

            // Compute triangle barycentric coordinates (pre-division).
            float u = n0.dot(norm);
            float v = n1.dot(norm);
            float w = n2.dot(norm);
            //float s = 1.0f / (u + v + w);
            //u *= s;
            //v *= s;
            //w *= s;

            if ((u >= 0.0f && v >= 0.0f && w >= 0.0f)) {
                // The ray intersects the triangle
                if (t < closest_t) {
                    // this is the new closest t value
                    res.t = t;
                    res.colliderID = hull->m_hullID;
                    res.contactPoint = Q;
                    res.normal = normal;

                    closest_t = t;
                }
            }
        }
    }

    return res;
}

void CollisionWorld::testCreate(ResourceManager* resource) {
    
    // Floor box
    CollisionHull hull1;
    hull1.loadVerts(resource, 8,
        vec3(-5, 0, -5),
        vec3(-5, 0,  5),
        vec3( 5, 0,  5),
        vec3( 5, 0, -5),
        vec3(-5, -2, -5),
        vec3(-5, -2,  5),
        vec3( 5, -2,  5),
        vec3( 5, -2, -5)
    );
    hull1.loadFaces(resource, 2,
        Triangle(0, 1, 2),
        Triangle(0, 2, 3)
        
        /*Edge(4, 5),
        Edge(5, 6),
        Edge(6, 7),
        Edge(7, 4),

        Edge(0, 4),
        Edge(1, 5),
        Edge(2, 6),
        Edge(3, 7),

        Edge(0, 2)*/
        );
    hull1.bufferData();
    m_static.push_back(hull1);

    // Square pyramid
    CollisionHull hull2;
    hull2.loadVerts(resource, 5,
        vec3(.5, 0, 1),
        vec3(-.5, 0, 1),
        vec3(-.5, 0, -1),
        vec3(.5, 0, -1),
        vec3(0, 2, 0)
    );
    hull2.loadFaces(resource, 6,
        Triangle(0, 1, 2),
        Triangle(0, 2, 3),
        Triangle(0, 1, 4),
        Triangle(1, 2, 4),
        Triangle(2, 3, 4),
        Triangle(3, 0, 4)
    );
    hull2.bufferData();
    hull2.position = vec3(-1.5, 0, 0);
    m_static.push_back(hull2);

    CollisionHull hull3;
    // Triangle pyramid
    hull3.loadVerts(resource, 4,
        vec3(-.5, 0, 0),
        vec3(.5, 0, -1),
        vec3(.5, 0, 1),
        vec3(0, 1, 0)
    );

    hull3.loadFaces(resource, 4,
        Triangle(0, 1, 2),
        Triangle(0, 1, 3),
        Triangle(1, 2, 3),
        Triangle(2, 0, 3)
    );
    hull3.bufferData();
    hull3.position = vec3(-1.5, 2.25, 0);
    m_dynamic.push_back(hull3);
}

UID_t CollisionWorld::CreateNewCubeHull(ResourceManager* resource, vec3 position, scalar size) {
    CollisionHull hull;

    scalar halfSize = size / 2.0f;

    // Box hull
    hull.loadVerts(resource, 8,
        vec3(-halfSize, -halfSize, -halfSize),
        vec3(-halfSize, -halfSize, halfSize),
        vec3(halfSize, -halfSize, halfSize),
        vec3(halfSize, -halfSize, -halfSize),

        vec3(-halfSize, halfSize, -halfSize),
        vec3(-halfSize, halfSize, halfSize),
        vec3(halfSize, halfSize, halfSize),
        vec3(halfSize, halfSize, -halfSize)
    );
    hull.loadFaces(resource, 12,
        Triangle(0, 1, 4),
        Triangle(4, 1, 5),
        Triangle(1, 2, 6),
        Triangle(1, 6, 5),
        Triangle(2, 3, 7),
        Triangle(2, 7, 6),

        Triangle(3, 0, 4),
        Triangle(3, 4, 7),
        Triangle(0, 2, 1),
        Triangle(0, 3, 2),
        Triangle(4, 5, 7),
        Triangle(7, 5, 6)
    );
    hull.bufferData();
    hull.position = position;
    m_static.push_back(hull);

    return hull.m_hullID;
}

UID_t CollisionWorld::CreateNewCapsule(ResourceManager* resource, 
    vec3 position, scalar height, scalar radius) {
    CollisionHull hull;

    // Box hull
    hull.loadVerts(resource, 3,
        vec3(0, 0, 0),
        vec3(0, height/2, 0), // to allow it to render more easily
        vec3(0, height, 0)
    );
    hull.loadFaces(resource, 1, // kind of hackey - should only be a line
        Triangle(0, 1, 2)
    );
    hull.bufferData();
    hull.position = position;
    hull.m_radius = radius;
    m_dynamic.push_back(hull);

    return hull.m_hullID;
}

CollisionHull* CollisionWorld::getHullFromID(UID_t id) {
    for (int n = 0; n < m_static.size(); n++) {
        auto hull = &m_static[n];
        if (hull->m_hullID == id)
            return hull;
    }
    for (int n = 0; n < m_dynamic.size(); n++) {
        auto hull = &m_dynamic[n];
        if (hull->m_hullID == id)
            return hull;
    }
    return nullptr;
}