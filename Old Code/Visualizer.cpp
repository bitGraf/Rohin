#include "Visualizer.hpp"

/*
// Compute the distance between two polygons using the GJK algorithm.
void Distance3D(gjk_Output* output, gjk_Input& input)
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

    // Begin recording the simplices for visualization.
    output->simplexCount = 0;
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
}
// why is there an include here
#include "Resource/ResourceManager.hpp"
void Visualizer::Init(ResourceManager* resource) {
}

void Visualizer::Step() {
    m_lines.clear();

    in.hull1 = &cWorld.m_dynamic[1];
    for (int n = 0; n < cWorld.m_static.size(); n++) {
        in.hull2 = &cWorld.m_static[n];

        Distance3D(&out, in);
        Line l = {out.point1, out.point2};
        m_lines.push_back(l);
    }

    res = cWorld.Raycast(cWorld.m_dynamic[0].position, vec3(0,-1,0), 0.5f);
}
*/