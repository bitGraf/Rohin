#include "GJK.hpp"

bool GJK(CollisionObject* col1, CollisionObject* col2, vec3* mtv) {
    vec3 a, b, c, d; // Simplex verts -> a is always most recent
    vec3 search_dir = col1->Position - col2->Position; // Initial search direction

                                                       // Get initial simplex point
    c = col2->support(search_dir) - col1->support(-search_dir);
    search_dir = -c;

    // Get second point
    b = col2->support(search_dir) - col1->support(-search_dir);

    if (b.dot(search_dir) < 0) { return false; } //didnt reach the origin, won't enclose it

                                                 // search perpendicular to line segment towards origin
    search_dir = (c - b).cross(-b).cross(c - b); // ((c-b)X(-b)) X (c-b)
    if (search_dir.length_2() < 0.001) { //origin on line segment
                                         // ? choose a normal search vector?
        search_dir = (c - b).cross(vec3(1, 0, 0)); // ? normal with x-axis
        if (search_dir.length_2() < 0.001)
            search_dir = (c - b).cross(vec3(0, 0, -1)); // ? normal with z-axis
    }
    int simp_dim = 2; //2-simplex

    for (int iterations = 0; iterations < GJK_MAX_ITERATIONS; iterations++) {
        a = col2->support(search_dir) - col1->support(-search_dir);
        if (a.dot(search_dir)<0) { return false; } //won't enclose the origin

        simp_dim++;
        if (simp_dim == 3) {
            update_simplex3(a, b, c, d, simp_dim, search_dir);
        }
        else if (update_simplex4(a, b, c, d, simp_dim, search_dir)) {
            if (mtv) *mtv = EPA(a, b, c, d, col1, col2);
            return true;
        }
    } // ! failed to converge in max number of iterations?
    return false;
}

// Triangle simplex
void update_simplex3(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &dim, vec3 &search_dir) {
    vec3 n = (b - a).cross(c - a);
    vec3 AO = -a;

    dim = 2;
    if ((b - a).cross(n).dot(AO)>0) {
        /* Take Edge AB */
        c = a;
        search_dir = (b - a).cross(AO).cross(b - a);
        return;
    }
    if (n.cross(c - a).dot(AO)>0) {
        /* Take Edge AC */
        b = a;
        search_dir = (c - a).cross(AO).cross(c - a);
        return;
    }

    dim = 3;
    /* Take triangle ABC */
    if (n.dot(AO)>0) {
        /* Above triangle */
        d = c;
        c = b;
        b = a;
        search_dir = n;
        return;
    }

    /* Below triangle */
    d = b;
    b = a;
    search_dir = -n;
    return;
}

// Tetrahedron simplex
bool update_simplex4(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &simp_dim, vec3 &search_dir) {
    // Get normals of three faces
    vec3 ABC = (b - a).cross(c - a);
    vec3 ACD = (c - a).cross(d - a);
    vec3 ADB = (d - a).cross(b - a);

    vec3 AO = -a;
    simp_dim = 3;

    if (ABC.dot(AO)>0) {
        d = c;
        c = b;
        b = a;
        search_dir = ABC;
        return false;
    }
    if (ACD.dot(AO)>0) {
        b = a;
        search_dir = ACD;
        return false;
    }
    if (ADB.dot(AO)>0) {
        c = d;
        d = b;
        b = a;
        search_dir = ADB;
        return false;
    }

    return true;
}

// EPA Algorithm
vec3 EPA(vec3 a, vec3 b, vec3 c, vec3 d, CollisionObject* col1, CollisionObject* col2) {
    return vec3(0, 1, 0);
}