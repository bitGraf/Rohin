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
    scalar closest_t = 1;

    res.start = start;
    res.end = end;
    res.t = 1;
    //res.colliderID = 0;
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
            auto face = &hull->faces.data[m];
            vec3 v0 = hull->GetVertWorldSpace(face->indices[0]);
            vec3 v1 = hull->GetVertWorldSpace(face->indices[1]);
            vec3 v2 = hull->GetVertWorldSpace(face->indices[2]);

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
            vec3 n1 = (v1 - Q).cross(v2);
            vec3 n2 = (v2 - Q).cross(v0);
            vec3 n3 = (v0 - Q).cross(v1);

            // Compute triangle barycentric coordinates (pre-division).
            float u = n1.dot(normal);
            float v = n2.dot(normal);
            float w = n3.dot(normal);

            if (u > 0.0f && v > 0.0f && w > 0.0f) {
                // The ray intersects the triangle
                if (t < closest_t) {
                    // this is the new closest t value
                    res.t = t;
                    //res.colliderID = hull->id;
                    res.contactPoint = Q;
                    res.normal = normal;
                }
            }
        }
    }

    return res;
}

void CollisionWorld::testCreate(ResourceManager* resource) {
    CollisionHull hull;

    // Floor box
    hull.loadVerts(resource, 8,
        vec3(-5, 0, -5),
        vec3(-5, 0,  5),
        vec3( 5, 0,  5),
        vec3( 5, 0, -5),
        vec3(-5, -2, -5),
        vec3(-5, -2,  5),
        vec3( 5, -2,  5),
        vec3( 5, -2, -5)
    );
    hull.loadFaces(resource, 13,
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
    hull.bufferData();
    m_static.push_back(hull);

    // Square pyramid
    hull.loadVerts(resource, 5,
        vec3(.5, 0, 1),
        vec3(-.5, 0, 1),
        vec3(-.5, 0, -1),
        vec3(.5, 0, -1),
        vec3(0, 2, 0)
    );
    hull.loadFaces(resource, 8,
        Triangle(0, 1, 2),
        Triangle(0, 2, 3),
        Triangle(0, 1, 4),
        Triangle(1, 2, 4),
        Triangle(2, 3, 4),
        Triangle(3, 0, 4)
    );
    hull.bufferData();
    hull.position = vec3(-1.5, 0, 0);
    m_static.push_back(hull);

    // Triangle pyramid
    hull.loadVerts(resource, 4,
        vec3(-.5, 0, 0),
        vec3(.5, 0, -1),
        vec3(.5, 0, 1),
        vec3(0, 1, 0)
    );

    hull.loadFaces(resource, 6,
        Triangle(0, 1, 2),
        Triangle(0, 1, 3),
        Triangle(1, 2, 3),
        Triangle(2, 0, 3)
    );
    hull.bufferData();
    hull.position = vec3(-1.5, 2.25, 0);
    m_dynamic.push_back(hull);
}