#include "Visualizer.hpp"

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
        default: {assert(false);} break;
    }

    return vec3();
}

void Simplex::GetWitnessPoints(vec3* point1, vec3* point2)
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
}

// Closest point on line segment to Q.
// Voronoi regions: A, B, AB
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
// Voronoi regions: A, B, C, AB, BC, CA, ABC
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
    vec3 n = (B-A).cross(C-A);
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

// Closest point on triangle to Q.
// Voronoi regions: A, B, C, D, AB, BC, CA, AD, BD, CD, ABC, ABD, DBC, CAD, ABCD
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
    n2 = C.cross(B);
    n3 = B.cross(D);

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
    float denom = (C-B).cross(A-B).dot(D-B);
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
void Distance3D(Output* output, gjk_Input& input)
{
    CollisionHull* polygon1 = &input.polygon1;
    CollisionHull* polygon2 = &input.polygon2;

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

    // Begin recording the simplices for visualization.
    output->simplexCount = 0;
    Output::TermCode term = Output::TermCode::e_maxIterations;

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
            term = Output::TermCode::e_intersect;
            break;
        }

        if (numItsIncreasing >= MAX_INCREASING_ITS) {
            term = Output::TermCode::e_distanceIncrease;
            break;
        }

        // Get search direction.
        vec3 d = simplex.GetSearchDirection();

        // Ensure the search direction non-zero.
        if (d.dot(d) <= 1e-15)
        {
            term = Output::TermCode::e_zeroSearch;
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
            term = Output::e_duplicate;
            break;
        }

        // New vertex is ok and needed.
        ++simplex.m_count;
    }

    // Prepare output.
    simplex.GetWitnessPoints(&output->point1, &output->point2);
    output->distance = (output->point1 - output->point2).length();
    output->iterations = iter;
    output->m_term = term;
}

#include "Resource\ResourceManager.hpp"
void Visualizer::Init(ResourceManager* resource) {
}

void Visualizer::Step() {
    m_lines.clear();

    in.polygon1 = cWorld.m_dynamic[0];
    for (int n = 0; n < cWorld.m_static.size(); n++) {
        in.polygon2 = cWorld.m_static[n];

        Distance3D(&out, in);
        Line l = {out.point1, out.point2};
        m_lines.push_back(l);
    }

    res = cWorld.Raycast(cWorld.m_dynamic[0].position, vec3(0,-1,0), 0.5f);
}