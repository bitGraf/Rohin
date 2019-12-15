#include "CollisionWorld.hpp"
#include "Resource\ResourceManager.hpp"

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

RaycastResult CollisionWorld::Raycast(UID_t target, vec3 start, vec3 end) {
    RaycastResult res;

    vec3 d = end - start;
    scalar closest_t = 2;

    res.start = start;
    res.end = end;
    res.t = closest_t;
    res.colliderID = 0;
    res.contactPoint = end;
    res.normal = vec3(0, 0, 0);

    auto hull = this->getHullFromID(target);

    // only select the ones that close
    vec3 toHull = hull->position - start;
    if (toHull.dot(d) < 0)
        return res;

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

    return res;
}

UID_t CollisionWorld::CreateNewCubeHull(ResourceManager* resource, vec3 position, scalar size) {
    CollisionHull hull;

    scalar halfSize = size / 2.0f;

    // Box hull
    vec3Array vblock = resource->reserveDataBlocks<vec3>(8);
    vblock.data[0] = vec3(-halfSize, -halfSize, -halfSize);
    vblock.data[1] = vec3(-halfSize, -halfSize, halfSize);
    vblock.data[2] = vec3(halfSize, -halfSize, halfSize);
    vblock.data[3] = vec3(halfSize, -halfSize, -halfSize);

    vblock.data[4] = vec3(-halfSize, halfSize, -halfSize);
    vblock.data[5] = vec3(-halfSize, halfSize, halfSize);
    vblock.data[6] = vec3(halfSize, halfSize, halfSize);
    vblock.data[7] = vec3(halfSize, halfSize, -halfSize);
    hull.vertices = vblock;

    FaceArray fblock = resource->reserveDataBlocks<Triangle>(12);
    fblock.data[0] = Triangle(0, 1, 4);
    fblock.data[1] = Triangle(4, 1, 5);
    fblock.data[2] = Triangle(1, 2, 6);
    fblock.data[3] = Triangle(1, 6, 5);
    fblock.data[4] = Triangle(2, 3, 7);
    fblock.data[5] = Triangle(2, 7, 6);

    fblock.data[6] = Triangle(3, 0, 4);
    fblock.data[7] = Triangle(3, 4, 7);
    fblock.data[8] = Triangle(0, 2, 1);
    fblock.data[9] = Triangle(0, 3, 2);
    fblock.data[10] = Triangle(4, 5, 7);
    fblock.data[11] = Triangle(7, 5, 6);
    hull.faces = fblock;

    hull.bufferData();
    hull.position = position;
    m_static.push_back(hull);

    return hull.m_hullID;
}

UID_t CollisionWorld::CreateNewCubeHull(ResourceManager* resource, 
    vec3 position, scalar xSize, scalar ySize, scalar zSize) {
    CollisionHull hull;

    scalar halfx = xSize / 2.0f;
    scalar halfy = ySize / 2.0f;
    scalar halfz = zSize / 2.0f;

    // Box hull
    vec3Array vBlock = resource->reserveDataBlocks<vec3>(8);
    vBlock.data[0] = vec3(-halfx, -halfy, -halfz);
    vBlock.data[0] = vec3(-halfx, -halfy, halfz);
    vBlock.data[0] = vec3(halfx, -halfy, halfz);
    vBlock.data[0] = vec3(halfx, -halfy, -halfz);

    vBlock.data[0] = vec3(-halfx, halfy, -halfz);
    vBlock.data[0] = vec3(-halfx, halfy, halfz);
    vBlock.data[0] = vec3(halfx, halfy, halfz);
    vBlock.data[0] = vec3(halfx, halfy, -halfz);
    hull.vertices = vBlock;

    FaceArray fBlock = resource->reserveDataBlocks<Triangle>(12);
    fBlock.data[0] = Triangle(0, 1, 4);
    fBlock.data[0] = Triangle(4, 1, 5);
    fBlock.data[0] = Triangle(1, 2, 6);
    fBlock.data[0] = Triangle(1, 6, 5);
    fBlock.data[0] = Triangle(2, 3, 7);
    fBlock.data[0] = Triangle(2, 7, 6);

    fBlock.data[0] = Triangle(3, 0, 4);
    fBlock.data[0] = Triangle(3, 4, 7);
    fBlock.data[0] = Triangle(0, 2, 1);
    fBlock.data[0] = Triangle(0, 3, 2);
    fBlock.data[0] = Triangle(4, 5, 7);
    fBlock.data[0] = Triangle(7, 5, 6);
    hull.faces = fBlock;

    hull.bufferData();
    hull.position = position;
    m_static.push_back(hull);

    return hull.m_hullID;
}

UID_t CollisionWorld::CreateNewCapsule(ResourceManager* resource, 
    vec3 position, scalar height, scalar radius) {
    CollisionHull hull;

    // Box hull
    vec3Array vBlock = resource->reserveDataBlocks<vec3>(3);
    vBlock.data[0] = vec3(0, 0, 0);
    vBlock.data[0] = vec3(0, height / 2, 0); // to allow it to render more easily
    vBlock.data[0] = vec3(0, height, 0);
    hull.vertices = vBlock;

    FaceArray fBlock = resource->reserveDataBlocks<Triangle>(1);
    fBlock.data[0] = Triangle(0, 1, 2);
    hull.faces = fBlock;

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

ShapecastResult_multi CollisionWorld::Shapecast_multi(UID_t id, vec3 vel) {
    ShapecastResult_multi res;
    res.lowestTOI = 1;
    res.numContacts = 0;
    res.planes[0].TOI = 1e10;
    CollisionHull* hull1 = this->getHullFromID(id);
    if (hull1 == nullptr) {
        return res;
    }

    for (int n = 0; n < m_static.size(); n++) {
        CollisionHull* hull2 = &m_static[n];
        if (hull1->m_hullID == hull2->m_hullID)
            continue; // don't check against itself

        int iters;
        ContactPlane plane;
        plane.colliderID = hull2->m_hullID;
        plane.TOI= TimeOfImpact(hull1, vel, hull2, vec3(),
            &plane.contact_normal, &plane.contact_point, &iters);
        bool placed = false;
        if (plane.TOI < 1) {
            if (res.numContacts == 0) {
                res.planes[0] = plane; // start the list
            }
            else {
                for (int n = 0; n < res.numContacts; n++) {
                    if (plane.TOI < res.planes[n].TOI) {
                        // shift entire list back once
                        for (int m = res.numContacts - 1; m >= n; m--) {
                            res.planes[m+1] = res.planes[m];
                        }

                        // insert element at position n
                        res.planes[n] = plane;
                        placed = true;
                        break;
                    }
                }

                if (!placed) {
                    res.planes[res.numContacts] = plane;
                }
            }

            res.numContacts++;
            if (res.numContacts > MAX_CONTACTS) {
                res.numContacts = MAX_CONTACTS;
            }
        }
    }

    return res;
}


ShapecastResult CollisionWorld::Shapecast(UID_t id, vec3 vel) {
    ShapecastResult res;
    res.TOI = 1e10;
    res.colliderID = 0;
    CollisionHull* hull1 = this->getHullFromID(id);
    if (hull1 == nullptr) {
        return res;
    }

    for (int n = 0; n < m_static.size(); n++) {
        CollisionHull* hull2 = &m_static[n];
        if (hull1->m_hullID == hull2->m_hullID)
            continue; // don't check against itself

        int iters;
        vec3 contact_normal, contact_point;
        float TOI = TimeOfImpact(hull1, vel, hull2, vec3(),
            &contact_normal, &contact_point, &iters);

        if (TOI < res.TOI) {
            // This is a lower time of impact
            res.TOI = TOI;
            res.colliderID = hull2->m_hullID;
            res.contact_normal = contact_normal;
            res.contact_point = contact_point;
            res.iters = iters;
        }
    }

    return res;
}


scalar CollisionWorld::TimeOfImpact(CollisionHull* hull1, vec3 vel1, CollisionHull* hull2, vec3 vel2,
    vec3* out_normal, vec3* out_contact_point, int* out_iterations) {
    assert(hull1 && hull2);

    float t = 0;
    vec3 a, b, n;

    float feather_radius = 1.0e-3f;
    float d = StepGJK(t, hull1, vel1, hull2, vel2, &a, &b, feather_radius); //closest distance b/w hulls
    vec3 v = vel2 - vel1;

    int iter = 0;
    float eps = 1.0e-6f;
    while (d > eps && t < 1) {
        ++iter;
        vec3 d_vec = (b - a).get_unit();
        float velocity_bound = abs(d_vec.dot(v)); //should prob just check if positive
        if (!velocity_bound) return 1; // moving perpendicular

        float delta = d / velocity_bound; // how far (in t units) hulls will move together.
        float t0 = t;
        float t1 = t + delta;

        if (t0 == t1) break;
        t = t1;

        d = StepGJK(t, hull1, vel1, hull2, vel2, &a, &b, feather_radius);
    }

    d = StepGJK(t, hull1, vel1, hull2, vel2, &a, &b, 0);
    n = (a - b).get_unit();
    t = t >= 1 ? 1 : t;

    vec3 p = (a + b) * 0.5f;

    if (n.length_2() == 0) {
        *out_normal = -v.get_unit();
    }
    else {
        *out_normal = n;
    }
    *out_contact_point = p;
    *out_iterations = iter;

    return t;
}

scalar CollisionWorld::StepGJK(float t, 
    CollisionHull* hull1, vec3 vel1, 
    CollisionHull* hull2, vec3 vel2,
    vec3* point1, vec3* point2,
    float feather_radius) {

    vec3 hull1Pos = hull1->position; //save for later
    vec3 hull2Pos = hull2->position;
    float radius = hull1->m_radius;

    hull1->position += vel1 * t; // move the object 't' along their velocities
    hull2->position += vel2 * t;

    hull1->m_radius += feather_radius;

    gjk_Input in;
    gjk_Output out;

    in.hull1 = hull1;
    in.hull2 = hull2;

    GJK(&out, in);
    *point1 = out.point1;
    *point2 = out.point2;

    if (out.m_hit) {
        bool stopper = true;
    }

    hull1->position = hull1Pos; // reset to actual values
    hull2->position = hull2Pos;
    hull1->m_radius = radius;

    return out.distance;
}









vec3 Simplex::GetSearchDirection() {
    // calcualte the search direction based on current simplex
    switch (m_count)
    {
    case 1: {
        return -m_vertexA.point;
    } break;
    case 2: {
        vec3 ba = m_vertexB.point - m_vertexA.point;
        vec3 b0 = -m_vertexB.point;
        vec3 t = ba.cross(b0);
        return t.cross(ba);
    } break;
    case 3: {
        vec3 ab = m_vertexA.point - m_vertexB.point;
        vec3 ac = m_vertexA.point - m_vertexC.point;
        vec3 n = ab.cross(ac);
        if (n.dot(m_vertexA.point) <= 0.0f) {
            return n;
        }
        else {
            return -n;
        }
    } break;
    default: {assert(false); } break;
    }

    return vec3();
}

void Simplex::GetWitnessPoints(vec3* point1, vec3* point2, float radius1, float radius2)
{
    switch (m_count) {
    case 1:
        *point1 = m_vertexA.point1;
        *point2 = m_vertexA.point2;
        break;
    case 2: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u);
        *point1 = (denom * m_vertexA.u) * m_vertexA.point1 + (denom * m_vertexB.u) * m_vertexB.point1;
        *point2 = (denom * m_vertexA.u) * m_vertexA.point2 + (denom * m_vertexB.u) * m_vertexB.point2;
    } break;
    case 3: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u + m_vertexC.u);
        *point1 = (denom * m_vertexA.u) * m_vertexA.point1 +
            (denom * m_vertexB.u) * m_vertexB.point1 +
            (denom * m_vertexC.u) * m_vertexC.point1;
        *point2 = (denom * m_vertexA.u) * m_vertexA.point2 +
            (denom * m_vertexB.u) * m_vertexB.point2 +
            (denom * m_vertexC.u) * m_vertexC.point2;
    } break;
    case 4: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u + m_vertexC.u + m_vertexD.u);
        *point1 = (denom * m_vertexA.u) * m_vertexA.point1 +
            (denom * m_vertexB.u) * m_vertexB.point1 +
            (denom * m_vertexC.u) * m_vertexC.point1 +
            (denom * m_vertexD.u) * m_vertexD.point1;
        *point2 = (denom * m_vertexA.u) * m_vertexA.point2 +
            (denom * m_vertexB.u) * m_vertexB.point2 +
            (denom * m_vertexC.u) * m_vertexC.point2 +
            (denom * m_vertexD.u) * m_vertexD.point2;
    } break;
    }

    if (m_hit) { // could record a hit that actually is a miss ?
        *point1 = *point2;
    }
    else {
        if (radius1 > 0.0f || radius2 > 0.0f) {
            vec3 dir = (*point1 - *point2).get_unit();

            *point1 -= (dir * radius1);
            *point2 += (dir * radius2);
        }
    }
}

// Closest point on line segment to Q.
void Simplex::Solve2(const vec3& Q)
{
    vec3 A = m_vertexA.point;
    vec3 B = m_vertexB.point;

    // Compute barycentric coordinates (pre-division).
    float u = (Q - B).dot(A - B);
    float v = (Q - A).dot(B - A);

    // Region A
    if (v <= 0.0f)
    {
        // Simplex is reduced to just vertex A.
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Region B
    if (u <= 0.0f)
    {
        // Simplex is reduced to just vertex B.
        // We move vertex B into vertex A and reduce the count.
        m_vertexA = m_vertexB;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Region AB. Due to the conditions above, we are
    // guaranteed the the edge has non-zero length and division
    // is safe.
    m_vertexA.u = u;
    m_vertexB.u = v;
    m_count = 2;
}

// Closest point on triangle to Q.
void Simplex::Solve3(const vec3& Q)
{
    // Get closest point on TriABC to Q
    vec3 A = m_vertexA.point;
    vec3 B = m_vertexB.point;
    vec3 C = m_vertexC.point;

    // Compute edge barycentric coordinates (pre-division).
    float uAB = (Q - B).dot(A - B);
    float vAB = (Q - A).dot(B - A);

    float uBC = (Q - C).dot(B - C);
    float vBC = (Q - B).dot(C - B);

    float uCA = (Q - A).dot(C - A);
    float vCA = (Q - C).dot(A - C);

    // Region A
    if (vAB <= 0.0f && uCA <= 0.0f)
    {
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Region B
    if (uAB <= 0.0f && vBC <= 0.0f)
    {
        m_vertexA = m_vertexB;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Region C
    if (uBC <= 0.0f && vCA <= 0.0f)
    {
        m_vertexA = m_vertexC;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Compute signed triangle area.
    vec3 n = (B - A).cross(C - A);
    vec3 n1 = B.cross(C);
    vec3 n2 = C.cross(A);
    vec3 n3 = A.cross(B);

    // Compute triangle barycentric coordinates (pre-division).
    float uABC = n1.dot(n);
    float vABC = n2.dot(n);
    float wABC = n3.dot(n);

    // Region AB
    if (uAB > 0.0f && vAB > 0.0f && wABC <= 0.0f)
    {
        m_vertexA.u = uAB;
        m_vertexB.u = vAB;
        m_count = 2;
        return;
    }

    // Region BC
    if (uBC > 0.0f && vBC > 0.0f && uABC <= 0.0f)
    {
        m_vertexA = m_vertexB;
        m_vertexB = m_vertexC;

        m_vertexA.u = uBC;
        m_vertexB.u = vBC;
        m_count = 2;
        return;
    }

    // Region CA
    if (uCA > 0.0f && vCA > 0.0f && vABC <= 0.0f)
    {
        m_vertexB = m_vertexA;
        m_vertexA = m_vertexC;

        m_vertexA.u = uCA;
        m_vertexB.u = vCA;
        m_count = 2;
        return;
    }

    // Region ABC
    // The triangle area is guaranteed to be non-zero.
    assert(uABC > 0.0f && vABC > 0.0f && wABC > 0.0f);
    m_vertexA.u = uABC;
    m_vertexB.u = vABC;
    m_vertexC.u = wABC;
    m_count = 3;
}

// Closest point on tetrahedron to Q.
void Simplex::Solve4(const vec3& Q)
{
    // Get closest point on TetraABCD to Q
    vec3 A = m_vertexA.point;
    vec3 B = m_vertexB.point;
    vec3 C = m_vertexC.point;
    vec3 D = m_vertexD.point;

    // Compute edge barycentric coordinates (pre-division).
    float uAB = (Q - B).dot(A - B);
    float vAB = (Q - A).dot(B - A);

    float uBC = (Q - C).dot(B - C);
    float vBC = (Q - B).dot(C - B);

    float uCA = (Q - A).dot(C - A);
    float vCA = (Q - C).dot(A - C);

    float uBD = (Q - D).dot(B - D);
    float vBD = (Q - B).dot(D - B);

    float uDC = (Q - C).dot(D - C);
    float vDC = (Q - D).dot(C - D);

    float uAD = (Q - D).dot(A - D);
    float vAD = (Q - A).dot(D - A);

    // Region A
    if (vAB <= 0.0f && uCA <= 0.0f && vAD <= 0.0) {
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }
    // Region B
    if (uAB <= 0.0f && vBC <= 0.0f && vBD <= 0.0f) {
        m_vertexA = m_vertexB;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }
    // Region C
    if (uBC <= 0.0f && vCA <= 0.0f && uDC <= 0.0f) {
        m_vertexA = m_vertexC;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }
    // Region D
    if (uBD <= 0.0f && vDC <= 0.0f && uAD <= 0.0f) {
        m_vertexA = m_vertexD;
        m_vertexA.u = 1.0f;
        m_count = 1;
        return;
    }

    // Compute fractional area: ADB
    vec3 n = (D - A).cross(B - A);
    vec3 n1 = D.cross(B);
    vec3 n2 = B.cross(A);
    vec3 n3 = A.cross(D);

    float uADB = n1.dot(n);
    float vADB = n2.dot(n);
    float wADB = n3.dot(n);

    // Compute fractional area: ACD
    n = (C - A).cross(D - A);
    n1 = C.cross(D);
    n2 = D.cross(A);
    n3 = A.cross(C);

    float uACD = n1.dot(n);
    float vACD = n2.dot(n);
    float wACD = n3.dot(n);

    // Compute fractional area: CBD
    n = (B - C).cross(D - C);
    n1 = B.cross(D);
    n2 = D.cross(C);
    n3 = C.cross(B);

    float uCBD = n1.dot(n);
    float vCBD = n2.dot(n);
    float wCBD = n3.dot(n);

    // Compute fractional area: ABC
    n = (B - A).cross(C - A);
    n1 = B.cross(C);
    n2 = C.cross(A);
    n3 = A.cross(B);

    float uABC = n1.dot(n);
    float vABC = n2.dot(n);
    float wABC = n3.dot(n);

    // Region AB
    if (wABC <= 0.0f && vADB <= 0.0f && uAB > 0.0f && vAB > 0.0f)
    {
        m_vertexA.u = uAB;
        m_vertexB.u = vAB;
        m_count = 2;
        return;
    }

    // Region BC
    if (uABC <= 0.0f && wCBD <= 0.0f && uBC > 0.0f && vBC > 0.0f)
    {
        m_vertexA = m_vertexB;
        m_vertexB = m_vertexC;

        m_vertexA.u = uBC;
        m_vertexB.u = vBC;
        m_count = 2;
        return;
    }

    // Region CA
    if (vABC <= 0.0f && wACD <= 0.0f && uCA > 0.0f && vCA > 0.0f)
    {
        m_vertexB = m_vertexA;
        m_vertexA = m_vertexC;

        m_vertexA.u = uCA;
        m_vertexB.u = vCA;
        m_count = 2;
        return;
    }

    // Region DC
    if (vCBD <= 0.0f && uACD <= 0.0f && uDC > 0.0f && vDC > 0.0f)
    {
        m_vertexA = m_vertexD;
        m_vertexB = m_vertexC;

        m_vertexA.u = uDC;
        m_vertexB.u = vDC;
        m_count = 2;
        return;
    }

    // Region AD
    if (vACD <= 0.0f && wADB <= 0.0f && uAD > 0.0f && vAD > 0.0f)
    {
        m_vertexB = m_vertexD;

        m_vertexA.u = uAD;
        m_vertexB.u = vAD;
        m_count = 2;
        return;
    }

    // Region BD
    if (uCBD <= 0.0f && uADB <= 0.0f && uBD > 0.0f && vBD > 0.0f)
    {
        m_vertexA = m_vertexB;
        m_vertexB = m_vertexD;

        m_vertexA.u = uBD;
        m_vertexB.u = vBD;
        m_count = 2;
        return;
    }

    // Calculate fractional volume: ABCD
    //A.cross(B).dot(C);
    float denom = (C - B).cross(A - B).dot(D - B);
    float volume = (denom == 0) ? 1.0f : 1.0f / denom;
    float uABCD = C.cross(D).dot(B) * volume;
    float vABCD = C.cross(A).dot(D) * volume;
    float wABCD = D.cross(A).dot(B) * volume;
    float xABCD = B.cross(A).dot(C) * volume;

    // Region ABC
    if (xABCD <= 0.0f && uABC > 0.0f && vABC > 0.0f && wABC > 0.0f)
    {
        m_vertexA.u = uABC;
        m_vertexB.u = vABC;
        m_vertexC.u = wABC;
        m_count = 3;
        return;
    }
    // Region CBD
    if (uABCD <= 0.0f && uCBD > 0.0f && vCBD > 0.0f && wCBD > 0.0f)
    {
        m_vertexA = m_vertexC;
        m_vertexC = m_vertexD;

        m_vertexA.u = uCBD;
        m_vertexB.u = vCBD;
        m_vertexC.u = wCBD;
        m_count = 3;
        return;
    }
    // Region ACD
    if (vABCD <= 0.0f && uACD > 0.0f && vACD > 0.0f && wACD > 0.0f)
    {
        m_vertexB = m_vertexC;
        m_vertexC = m_vertexD;

        m_vertexA.u = uACD;
        m_vertexB.u = vACD;
        m_vertexC.u = wACD;
        m_count = 3;
        return;
    }
    // Region ADB
    if (wABCD <= 0.0f && uADB > 0.0f && vADB > 0.0f && wADB > 0.0f)
    {
        m_vertexC = m_vertexB;
        m_vertexB = m_vertexD;

        m_vertexA.u = uADB;
        m_vertexB.u = vADB;
        m_vertexC.u = wADB;
        m_count = 3;
        return;
    }

    // Has to be Region ABCD
    assert(uABCD > 0.0f && vABCD > 0.0f && wABCD > 0.0f && xABCD > 0.0f);
    m_vertexA.u = uABCD;
    m_vertexB.u = vABCD;
    m_vertexC.u = wABCD;
    m_vertexD.u = xABCD;
    m_count = 4;
    return;
}

float Simplex::GetDistance() {
    vec3 pnt;

    switch (m_count) {
    case 1:
        pnt = m_vertexA.point;
        break;
    case 2: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u);
        vec3 a = m_vertexA.point * denom * m_vertexA.u;
        vec3 b = m_vertexB.point * denom * m_vertexB.u;
        pnt = a + b;
    } break;
    case 3: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u + m_vertexC.u);
        vec3 a = m_vertexA.point * denom * m_vertexA.u;
        vec3 b = m_vertexB.point * denom * m_vertexB.u;
        vec3 c = m_vertexC.point * denom * m_vertexC.u;
        pnt = a + b + c;
    } break;
    case 4: {
        float denom = 1.0f / (m_vertexA.u + m_vertexB.u + m_vertexC.u + m_vertexD.u);
        vec3 a = m_vertexA.point * denom * m_vertexA.u;
        vec3 b = m_vertexB.point * denom * m_vertexB.u;
        vec3 c = m_vertexC.point * denom * m_vertexC.u;
        vec3 d = m_vertexD.point * denom * m_vertexD.u;
        pnt = a + b + c + d;
    } break;
    }

    return pnt.dot(pnt);
}



// Compute the distance between two polygons using the GJK algorithm.
void CollisionWorld::GJK(gjk_Output* output, gjk_Input& input)
{
    CollisionHull* polygon1 = input.hull1;
    CollisionHull* polygon2 = input.hull2;

    // Initialize the simplex.
    Simplex simplex;
    simplex.m_vertexA.index1 = 0;
    simplex.m_vertexA.index2 = 0;
    simplex.m_vertexA.point1 = polygon1->GetVertWorldSpace(0);
    simplex.m_vertexA.point2 = polygon2->GetVertWorldSpace(0);
    simplex.m_vertexA.point = simplex.m_vertexA.point2 - simplex.m_vertexA.point1;
    simplex.m_vertexA.u = 1.0f;
    simplex.m_vertexA.index1 = 0;
    simplex.m_vertexA.index2 = 0;
    simplex.m_count = 1;
    simplex.m_distance = 1e10; // large num
    simplex.m_hit = false;

                               // Begin recording the simplices for visualization.
    output->simplexCount = 0;
    output->m_hit = false;
    gjk_Output::TermCode term = gjk_Output::TermCode::e_maxIterations;

    // Get simplex vertices as an array.
    simplexVertex* vertices = &simplex.m_vertexA;

    // These store the vertices of the last simplex so that we
    // can check for duplicates and prevent cycling.
    int save1[4], save2[4];
    int saveCount = 0;

    int numItsIncreasing = 0;

    // Main iteration loop.
    const int k_maxIters = 20;
    int iter = 0;
    while (iter < k_maxIters)
    {
        // Copy simplex so we can identify duplicates.
        saveCount = simplex.m_count;
        for (int i = 0; i < saveCount; ++i)
        {
            save1[i] = vertices[i].index1;
            save2[i] = vertices[i].index2;
        }

        // Determine the closest point on the simplex and
        // remove unused vertices.
        switch (simplex.m_count)
        {
        case 1:
            break;

        case 2:
            simplex.Solve2(vec3(0.0f, 0.0f, 0.0f));
            break;

        case 3:
            simplex.Solve3(vec3(0.0f, 0.0f, 0.0f));
            break;

        case 4:
            simplex.Solve4(vec3(0.0f, 0.0f, 0.0f));
            break;

        default:
            assert(false);
        }

        float d2 = simplex.GetDistance();
        if (d2 >= simplex.m_distance) {
            numItsIncreasing++;
        }
        simplex.m_distance = d2;

        // Record for visualization.
        output->simplices[output->simplexCount++] = simplex;

        // If we have 4 points, then the origin is in the corresponding tetrahedron.
        if (simplex.m_count == 4)
        {
            term = gjk_Output::TermCode::e_intersect;
            simplex.m_hit = true;
            break;
        }

        if (numItsIncreasing >= MAX_INCREASING_ITS) {
            term = gjk_Output::TermCode::e_distanceIncrease;
            break;
        }

        // Get search direction.
        vec3 d = simplex.GetSearchDirection();

        // Ensure the search direction non-zero.
        if (d.dot(d) <= 1e-15)
        {
            term = gjk_Output::TermCode::e_zeroSearch;
            break;
        }

        // Compute a tentative new simplex vertex using support points.
        simplexVertex* vertex = vertices + simplex.m_count;
        vertex->index1 = polygon1->GetSupport(polygon1->rotation.getTranspose() * (-d));
        vertex->point1 = polygon1->GetVertWorldSpace(vertex->index1);
        vertex->index2 = polygon2->GetSupport(polygon2->rotation.getTranspose() * (d));
        vertex->point2 = polygon2->GetVertWorldSpace(vertex->index2);
        vertex->point = vertex->point2 - vertex->point1;

        // Iteration count is equated to the number of support point calls.
        ++iter;

        // Check for duplicate support points. This is the main termination criteria.
        bool duplicate = false;
        for (int i = 0; i < saveCount; ++i)
        {
            if (vertex->index1 == save1[i] && vertex->index2 == save2[i])
            {
                duplicate = true;
                break;
            }
        }

        // If we found a duplicate support point we must exit to avoid cycling.
        if (duplicate)
        {
            term = gjk_Output::e_duplicate;
            break;
        }

        // New vertex is ok and needed.
        ++simplex.m_count;
    }

    // Prepare output.
    simplex.GetWitnessPoints(&output->point1, &output->point2, polygon1->m_radius, polygon2->m_radius);
    output->distance = (output->point1 - output->point2).length();
    output->iterations = iter;
    output->m_term = term;
    output->m_hit = simplex.m_hit;
}