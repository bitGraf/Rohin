#ifndef _GJK_H_
#define _GJK_H_

#include "GameObject\CollisionObject.hpp"

#define GJK_MAX_ITERATIONS 64

#define EPA_TOLERANCE 0.01
#define EPA_MAX_NUM_FACES 128
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64

/**
* @brief Main GJK function
*
* @param col1 Collider 1
* @param col2 Collider 2
* @param Output: mtv Minimum Translation Vector
* @return true Colliders are intersecting. mtv is valid
* @return false No intersection
*/
bool GJK(CollisionObject* col1, CollisionObject* col2, vec3* mtv = NULL);

void update_simplex3(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &dim, vec3 &search_dir);
bool update_simplex4(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &dim, vec3 &search_dir);

/**
* @brief Main EPA function. Should only run when colliders are interescting
*
* @param a,b,c,d Simplex vertices - MUST enclose the origin, output of GJK algorithm
* @param col1,col2 Colliders
* @return vec3 minimum translation vector
*/
vec3 EPA(vec3 a, vec3 b, vec3 c, vec3 d, CollisionObject* col1, CollisionObject* col2);

#endif