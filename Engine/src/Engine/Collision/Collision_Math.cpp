#include "Collision.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define max3(a,b,c) ((((a)>(b))&&((a)>(c))) ? (a) : (((b)>(c)) ? (b) : (c)))
#define min3(a,b,c) ((((a)<(b))&&((a)<(c))) ? (a) : (((b)<(c)) ? (b) : (c)))

#if USING_GPU_GEMS_3_INTERSECTION
// Cube-Triangle intersection code taken from Graphics Gems III - V.7 TRIANGLE-CUBE INTERSECTION appendix sample
// https://github.com/erich666/GraphicsGems/blob/master/gemsiii/triangleCube.c

#define TRI_CUBE_OLD_TEST

#define EPS 10e-9
#ifdef TRI_CUBE_OLD_TEST
#define SIGN3( A ) (((A).x<0)?4:0 | ((A).y<0)?2:0 | ((A).z<0)?1:0)
#else
#define SIGN3( A ) \
    ((((A).x < EPS) ? 4 : 0) | (((A).x > -EPS) ? 32 : 0) | \
    (((A).y < EPS) ? 2 : 0) | (((A).y > -EPS) ? 16 : 0) | \
    (((A).z < EPS) ? 1 : 0) | (((A).z > -EPS) ? 8 : 0))
#endif

#define CROSS( A, B, C ) { \
    (C).x =  (A).y * (B).z - (A).z * (B).y; \
    (C).y = -(A).x * (B).z + (A).z * (B).x; \
    (C).z =  (A).x * (B).y - (A).y * (B).x; \
    }
#define SUB( A, B, C ) { \
    (C).x =  (A).x - (B).x; \
    (C).y =  (A).y - (B).y; \
    (C).z =  (A).z - (B).z; \
    }
#define LERP( A, B, C) ((B)+(A)*((C)-(B)))
#define MIN3(a,b,c) ((((a)<(b))&&((a)<(c))) ? (a) : (((b)<(c)) ? (b) : (c)))
#define MAX3(a,b,c) ((((a)>(b))&&((a)>(c))) ? (a) : (((b)>(c)) ? (b) : (c)))

// Which of the six face-plane(s) is point P outside of?
int32 face_plane(laml::Vec3 p) {
    int32 outcode;

    outcode = 0;
    if (p.x >  .5) outcode |= 0x01;
    if (p.x < -.5) outcode |= 0x02;
    if (p.y >  .5) outcode |= 0x04;
    if (p.y < -.5) outcode |= 0x08;
    if (p.z >  .5) outcode |= 0x10;
    if (p.z < -.5) outcode |= 0x20;
    return(outcode);
}

// Which of the twelve edge plane(s) is point P outside of?
int32 bevel_2d(laml::Vec3 p) {
    int32 outcode;

    outcode = 0;
    if ( p.x + p.y > 1.0) outcode |= 0x001;
    if ( p.x - p.y > 1.0) outcode |= 0x002;
    if (-p.x + p.y > 1.0) outcode |= 0x004;
    if (-p.x - p.y > 1.0) outcode |= 0x008;
    if ( p.x + p.z > 1.0) outcode |= 0x010;
    if ( p.x - p.z > 1.0) outcode |= 0x020;
    if (-p.x + p.z > 1.0) outcode |= 0x040;
    if (-p.x - p.z > 1.0) outcode |= 0x080;
    if ( p.y + p.z > 1.0) outcode |= 0x100;
    if ( p.y - p.z > 1.0) outcode |= 0x200;
    if (-p.y + p.z > 1.0) outcode |= 0x400;
    if (-p.y - p.z > 1.0) outcode |= 0x800;
    return(outcode);
}

// Which of the eight corner plane(s) is point P outside of?
int32 bevel_3d(laml::Vec3 p) {
    int32 outcode;

    outcode = 0;
    if (( p.x + p.y + p.z) > 1.5) outcode |= 0x01;
    if (( p.x + p.y - p.z) > 1.5) outcode |= 0x02;
    if (( p.x - p.y + p.z) > 1.5) outcode |= 0x04;
    if (( p.x - p.y - p.z) > 1.5) outcode |= 0x08;
    if ((-p.x + p.y + p.z) > 1.5) outcode |= 0x10;
    if ((-p.x + p.y - p.z) > 1.5) outcode |= 0x20;
    if ((-p.x - p.y + p.z) > 1.5) outcode |= 0x40;
    if ((-p.x - p.y - p.z) > 1.5) outcode |= 0x80;
    return(outcode);
}

// Test the point "alpha" of the way from P1 to P2
// See if it is on a face of the cube
// Consider only faces in "mask"
int32 check_point(laml::Vec3 p1, laml::Vec3 p2, real32 alpha, int32 mask) {
    laml::Vec3 plane_point;

    plane_point.x = LERP(alpha, p1.x, p2.x);
    plane_point.y = LERP(alpha, p1.y, p2.y);
    plane_point.z = LERP(alpha, p1.z, p2.z);
    return(face_plane(plane_point) & mask);
}

// Compute intersection of P1 --> P2 line segment with face planes
// Then test intersection point to see if it is on cube face      
// Consider only face planes in "outcode_diff"                    
// Note: Zero bits in "outcode_diff" means face line is outside of
int32 check_line(laml::Vec3 p1, laml::Vec3 p2, int32 outcode_diff) {

    if ((0x01 & outcode_diff) != 0)
        if (check_point(p1, p2, ( 0.5f - p1.x) / (p2.x - p1.x), 0x3e) == INSIDE) return(INSIDE);
    if ((0x02 & outcode_diff) != 0)
        if (check_point(p1, p2, (-0.5f - p1.x) / (p2.x - p1.x), 0x3d) == INSIDE) return(INSIDE);
    if ((0x04 & outcode_diff) != 0)
        if (check_point(p1, p2, ( 0.5f - p1.y) / (p2.y - p1.y), 0x3b) == INSIDE) return(INSIDE);
    if ((0x08 & outcode_diff) != 0)
        if (check_point(p1, p2, (-0.5f - p1.y) / (p2.y - p1.y), 0x37) == INSIDE) return(INSIDE);
    if ((0x10 & outcode_diff) != 0)
        if (check_point(p1, p2, ( 0.5f - p1.z) / (p2.z - p1.z), 0x2f) == INSIDE) return(INSIDE);
    if ((0x20 & outcode_diff) != 0)
        if (check_point(p1, p2, (-0.5f - p1.z) / (p2.z - p1.z), 0x1f) == INSIDE) return(INSIDE);
    return(OUTSIDE);
}

// Test if 3D point is inside 3D triangle
int32 point_triangle_intersection(laml::Vec3 p, collision_triangle t)
{
    int32 sign12, sign23, sign31;
    laml::Vec3 vect12, vect23, vect31, vect1h, vect2h, vect3h;
    laml::Vec3 cross12_1p, cross23_2p, cross31_3p;

    // First, a quick bounding-box test:
    // If P is outside triangle bbox, there cannot be an intersection.
    if (p.x > MAX3(t.v1.x, t.v2.x, t.v3.x)) return(OUTSIDE);
    if (p.y > MAX3(t.v1.y, t.v2.y, t.v3.y)) return(OUTSIDE);
    if (p.z > MAX3(t.v1.z, t.v2.z, t.v3.z)) return(OUTSIDE);
    if (p.x < MIN3(t.v1.x, t.v2.x, t.v3.x)) return(OUTSIDE);
    if (p.y < MIN3(t.v1.y, t.v2.y, t.v3.y)) return(OUTSIDE);
    if (p.z < MIN3(t.v1.z, t.v2.z, t.v3.z)) return(OUTSIDE);

    // For each triangle side, make a vector out of it by subtracting vertexes;
    // make another vector from one vertex to point P.                         
    // The crossproduct of these two vectors is orthogonal to both and the     
    // signs of its X,Y,Z components indicate whether P was to the inside or   
    // to the outside of this triangle side.                                   
    SUB(t.v1, t.v2, vect12);
    SUB(t.v1, p, vect1h);
    CROSS(vect12, vect1h, cross12_1p);
    sign12 = SIGN3(cross12_1p);      /* Extract X,Y,Z signs as 0..7 or 0...63 integer */

    SUB(t.v2, t.v3, vect23);
    SUB(t.v2, p, vect2h);
    CROSS(vect23, vect2h, cross23_2p);
    sign23 = SIGN3(cross23_2p);

    SUB(t.v3, t.v1, vect31);
    SUB(t.v3, p, vect3h);
    CROSS(vect31, vect3h, cross31_3p);
    sign31 = SIGN3(cross31_3p);

    // If all three crossproduct vectors agree in their component signs,
    // then the point must be inside all three.                         
    // P cannot be OUTSIDE all three sides simultaneously.              

    // this is the old test; with the revised SIGN3() macro, the test
    // needs to be revised.
#ifdef TRI_CUBE_OLD_TEST
    if ((sign12 == sign23) && (sign23 == sign31))
        return(INSIDE);
    else
        return(OUTSIDE);
#else
    return ((sign12 & sign23 & sign31) == 0) ? OUTSIDE : INSIDE;
#endif
}

/**********************************************/
/* This is the main algorithm procedure.      */
/* Triangle t is compared with a unit cube,   */
/* centered on the origin.                    */
/* It returns INSIDE (1) or OUTSIDE(0) if t   */
/* intersects or does not intersect the cube. */
/**********************************************/
bool32 triangle_cube_intersect(collision_triangle t)
{
    int32 v1_test, v2_test, v3_test;
    real32 d, denom;
    laml::Vec3 vect12, vect13, norm;
    laml::Vec3 hitpp, hitpn, hitnp, hitnn;

    /* First compare all three vertexes with all six face-planes */
    /* If any vertex is inside the cube, return immediately!     */

    if ((v1_test = face_plane(t.v1)) == INSIDE) return(INSIDE);
    if ((v2_test = face_plane(t.v2)) == INSIDE) return(INSIDE);
    if ((v3_test = face_plane(t.v3)) == INSIDE) return(INSIDE);

    /* If all three vertexes were outside of one or more face-planes, */
    /* return immediately with a trivial rejection!                   */

    if ((v1_test & v2_test & v3_test) != 0) return(OUTSIDE);

    /* Now do the same trivial rejection test for the 12 edge planes */

    v1_test |= bevel_2d(t.v1) << 8;
    v2_test |= bevel_2d(t.v2) << 8;
    v3_test |= bevel_2d(t.v3) << 8;
    if ((v1_test & v2_test & v3_test) != 0) return(OUTSIDE);

    /* Now do the same trivial rejection test for the 8 corner planes */

    v1_test |= bevel_3d(t.v1) << 24;
    v2_test |= bevel_3d(t.v2) << 24;
    v3_test |= bevel_3d(t.v3) << 24;
    if ((v1_test & v2_test & v3_test) != 0) return(OUTSIDE);

    /* If vertex 1 and 2, as a pair, cannot be trivially rejected */
    /* by the above tests, then see if the v1-->v2 triangle edge  */
    /* intersects the cube.  Do the same for v1-->v3 and v2-->v3. */
    /* Pass to the intersection algorithm the "OR" of the outcode */
    /* bits, so that only those cube faces which are spanned by   */
    /* each triangle edge need be tested.                         */

    if ((v1_test & v2_test) == 0)
        if (check_line(t.v1, t.v2, v1_test | v2_test) == INSIDE) return(INSIDE);
    if ((v1_test & v3_test) == 0)
        if (check_line(t.v1, t.v3, v1_test | v3_test) == INSIDE) return(INSIDE);
    if ((v2_test & v3_test) == 0)
        if (check_line(t.v2, t.v3, v2_test | v3_test) == INSIDE) return(INSIDE);

    /* By now, we know that the triangle is not off to any side,     */
    /* and that its sides do not penetrate the cube.  We must now    */
    /* test for the cube intersecting the interior of the triangle.  */
    /* We do this by looking for intersections between the cube      */
    /* diagonals and the triangle...first finding the intersection   */
    /* of the four diagonals with the plane of the triangle, and     */
    /* then if that intersection is inside the cube, pursuing        */
    /* whether the intersection point is inside the triangle itself. */

    /* To find plane of the triangle, first perform crossproduct on  */
    /* two triangle side vectors to compute the normal vector.       */

    SUB(t.v1,t.v2,vect12);
    SUB(t.v1,t.v3,vect13);
    CROSS(vect12, vect13, norm);

    /* The normal vector "norm" X,Y,Z components are the coefficients */
    /* of the triangles AX + BY + CZ + D = 0 plane equation.  If we   */
    /* solve the plane equation for X=Y=Z (a diagonal), we get        */
    /* -D/(A+B+C) as a metric of the distance from cube center to the */
    /* diagonal/plane intersection.  If this is between -0.5 and 0.5, */
    /* the intersection is inside the cube.  If so, we continue by    */
    /* doing a point/triangle intersection.                           */
    /* Do this for all four diagonals.                                */

    d = norm.x * t.v1.x + norm.y * t.v1.y + norm.z * t.v1.z;

    /* if one of the diagonals is parallel to the plane, the other will intersect the plane */
    if (fabs(denom = (norm.x + norm.y + norm.z)) > EPS)
        /* skip parallel diagonals to the plane; division by 0 can occur */
    {
        hitpp.x = hitpp.y = hitpp.z = d / denom;
        if (fabs(hitpp.x) <= 0.5)
            if (point_triangle_intersection(hitpp, t) == INSIDE) return(INSIDE);
    }
    if (fabs(denom = (norm.x + norm.y - norm.z)) > EPS)
    {
        hitpn.z = -(hitpn.x = hitpn.y = d / denom);
        if (fabs(hitpn.x) <= 0.5)
            if (point_triangle_intersection(hitpn, t) == INSIDE) return(INSIDE);
    }
    if (fabs(denom = (norm.x - norm.y + norm.z)) > EPS)
    {
        hitnp.y = -(hitnp.x = hitnp.z = d / denom);
        if (fabs(hitnp.x) <= 0.5)
            if (point_triangle_intersection(hitnp, t) == INSIDE) return(INSIDE);
    }
    if (fabs(denom = (norm.x - norm.y - norm.z)) > EPS)
    {
        hitnn.y = hitnn.z = -(hitnn.x = d / denom);
        if (fabs(hitnn.x) <= 0.5)
            if (point_triangle_intersection(hitnn, t) == INSIDE) return(INSIDE);
    }

    /* No edge touched the cube; no cube diagonal touched the triangle. */
    /* We're done...there was no intersection.                          */

    return(OUTSIDE);
}


#else


/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

/*
    Copyright 2020 Tomas Akenine-Möller

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
    documentation files (the "Software"), to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
    to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial
    portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
    OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
    OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    */

#include <math.h>
//#include <stdio.h>
#include "Engine/Memory/Memory.h"

#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
    dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
    dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
    dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
    dest[0]=v1[0]-v2[0]; \
    dest[1]=v1[1]-v2[1]; \
    dest[2]=v1[2]-v2[2];

#define FINDMINMAX(x0,x1,x2,min,max) \
    min = max = x0;   \
    if(x1<min) min=x1;\
    if(x1>max) max=x1;\
    if(x2<min) min=x2;\
    if(x2>max) max=x2;

int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])	// -NJMP-
{
    int q;
    float vmin[3],vmax[3],v;
    for(q=X;q<=Z;q++)
    {
        v=vert[q];					// -NJMP-
        if(normal[q]>0.0f)
        {
            vmin[q]=-maxbox[q] - v;	// -NJMP-
            vmax[q]= maxbox[q] - v;	// -NJMP-
        }
        else
        {
            vmin[q]= maxbox[q] - v;	// -NJMP-
            vmax[q]=-maxbox[q] - v;	// -NJMP-
        }
    }

    if(DOT(normal,vmin)>0.0f) return 0;	// -NJMP-
    if(DOT(normal,vmax)>=0.0f) return 1;	// -NJMP-

    return 0;
}

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			       	   \
    p2 = a*v2[Y] - b*v2[Z];			       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			           \
    p1 = a*v1[Y] - b*v1[Z];			       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p2 = -a*v2[X] + b*v2[Z];	       	       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p1 = -a*v1[X] + b*v1[Z];	     	       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
    p1 = a*v1[X] - b*v1[Y];			           \
    p2 = a*v2[X] - b*v2[Y];			       	   \
    if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
    p0 = a*v0[X] - b*v0[Y];				   \
    p1 = a*v1[X] - b*v1[Y];			           \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{
    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */
    float v0[3],v1[3],v2[3];
    //   float axis[3];
    float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
    float normal[3],e0[3],e1[3],e2[3];

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    SUB(v0,triverts[0],boxcenter);
    SUB(v1,triverts[1],boxcenter);
    SUB(v2,triverts[2],boxcenter);

    /* compute triangle edges */
    SUB(e0,v1,v0);      /* tri edge 0 */
    SUB(e1,v2,v1);      /* tri edge 1 */
    SUB(e2,v0,v2);      /* tri edge 2 */

    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabsf(e0[X]);
    fey = fabsf(e0[Y]);
    fez = fabsf(e0[Z]);
    AXISTEST_X01(e0[Z], e0[Y], fez, fey);
    AXISTEST_Y02(e0[Z], e0[X], fez, fex);
    AXISTEST_Z12(e0[Y], e0[X], fey, fex);

    fex = fabsf(e1[X]);
    fey = fabsf(e1[Y]);
    fez = fabsf(e1[Z]);
    AXISTEST_X01(e1[Z], e1[Y], fez, fey);
    AXISTEST_Y02(e1[Z], e1[X], fez, fex);
    AXISTEST_Z0(e1[Y], e1[X], fey, fex);

    fex = fabsf(e2[X]);
    fey = fabsf(e2[Y]);
    fez = fabsf(e2[Z]);
    AXISTEST_X2(e2[Z], e2[Y], fez, fey);
    AXISTEST_Y1(e2[Z], e2[X], fez, fex);
    AXISTEST_Z12(e2[Y], e2[X], fey, fex);

    /* Bullet 1: */
    /*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */

    /* test in X-direction */
    FINDMINMAX(v0[X],v1[X],v2[X],min,max);
    if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

    /* test in Y-direction */
    FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
    if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

    /* test in Z-direction */
    FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
    if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    CROSS(normal,e0,e1);
    // -NJMP- (line removed here)
    if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	// -NJMP-

    return 1;   /* box and triangle overlaps */
}

#if 1
bool32 triangle_cube_intersect(collision_triangle t, laml::Vec3 box_center, laml::Vec3 box_halfsize) {
    float triverts[3][3];
    memory_copy(triverts[0], t.v1._data, 3*sizeof(float));
    memory_copy(triverts[1], t.v2._data, 3*sizeof(float));
    memory_copy(triverts[2], t.v3._data, 3*sizeof(float));

    return triBoxOverlap(box_center._data, box_halfsize._data, triverts);
}
#else

bool32 sat_test(laml::Vec3 v0, laml::Vec3 v1, laml::Vec3 v2, laml::Vec3 n) {
    real32 e0, e1, e2;
    e0 = e1 = e2 = 0.25f;

    laml::Vec3 u0(1.0f, 0.0f, 0.0f);
    laml::Vec3 u1(0.0f, 1.0f, 0.0f);
    laml::Vec3 u2(0.0f, 0.0f, 1.0f);

    real32 p0 = laml::dot(v0, n);
    real32 p1 = laml::dot(v1, n);
    real32 p2 = laml::dot(v2, n);
    real32 r = e0*abs(laml::dot(u0, n)) + e1*abs(laml::dot(u1, n)) + e2*abs(laml::dot(u1, n));

    real32 pmin = min3(p0, p1, p2);
    real32 pmax = max3(p0, p1, p2);

    bool32 intersecting;
    if (pmax < -r || pmin > r) {
        intersecting = false;
    } else {
        intersecting = false;
    }

    return intersecting;
}

bool32 triangle_cube_intersect(collision_triangle t, laml::Vec3 box_center, laml::Vec3 box_halfsize) {
    real32 e0 = box_halfsize.x;
    real32 e1 = box_halfsize.y;
    real32 e2 = box_halfsize.z;

    real32 p0, p1, r;

    // translate triangle as conceptually moving aabb to origin
    laml::Vec3 v0 = t.v1 - box_center;
    laml::Vec3 v1 = t.v2 - box_center;
    laml::Vec3 v2 = t.v3 - box_center;

    // compute edge vectors for triangle
    laml::Vec3 f0 = laml::normalize(v1 - v0);
    laml::Vec3 f1 = laml::normalize(v2 - v1);
    laml::Vec3 f2 = laml::normalize(v0 - v2);

    laml::Vec3 u0(1.0f, 0.0f, 0.0f);
    laml::Vec3 u1(0.0f, 1.0f, 0.0f);
    laml::Vec3 u2(0.0f, 0.0f, 1.0f);

    real32 l;
    RH_INFO("start");
    laml::Vec3 a00 = laml::cross(u0, f0);
    l = laml::length_sq(a00);
    if (laml::length_sq(a00) < laml::eps<real32>) RH_INFO("ignore plane 1");
    laml::Vec3 a01 = laml::cross(u0, f1);
    l = laml::length_sq(a01);
    if (laml::length_sq(a01) < laml::eps<real32>) RH_INFO("ignore plane 2");
    laml::Vec3 a02 = laml::cross(u0, f2);
    l = laml::length_sq(a02);
    if (laml::length_sq(a02) < laml::eps<real32>) RH_INFO("ignore plane 3");
    laml::Vec3 a10 = laml::cross(u1, f0);
    l = laml::length_sq(a10);
    if (laml::length_sq(a10) < laml::eps<real32>) RH_INFO("ignore plane 4");
    laml::Vec3 a11 = laml::cross(u1, f1);
    l = laml::length_sq(a11);
    if (laml::length_sq(a11) < laml::eps<real32>) RH_INFO("ignore plane 5");
    laml::Vec3 a12 = laml::cross(u1, f2);
    l = laml::length_sq(a12);
    if (laml::length_sq(a12) < laml::eps<real32>) RH_INFO("ignore plane 6");
    laml::Vec3 a20 = laml::cross(u2, f0);
    l = laml::length_sq(a20);
    if (laml::length_sq(a20) < laml::eps<real32>) RH_INFO("ignore plane 7");
    laml::Vec3 a21 = laml::cross(u2, f1);
    l = laml::length_sq(a21);
    if (laml::length_sq(a21) < laml::eps<real32>) RH_INFO("ignore plane 8");
    laml::Vec3 a22 = laml::cross(u2, f2);
    l = laml::length_sq(a22);
    if (laml::length_sq(a22) < laml::eps<real32>) RH_INFO("ignore plane 9");
    RH_INFO("end");

    //sat_test(v0, v1, v2, a00);
    //sat_test(v0, v1, v2, a01);
    //sat_test(v0, v1, v2, a02);
    //
    //sat_test(v0, v1, v2, a10);
    //sat_test(v0, v1, v2, a11);
    //sat_test(v0, v1, v2, a12);
    //
    //sat_test(v0, v1, v2, a20);
    //sat_test(v0, v1, v2, a21);
    //sat_test(v0, v1, v2, a22);

    // test axes a00..a22 (category 3)
    // a00 = (0, -f0z, f0y)
    p0 = v0.z*v1.y - v0.y*v1.z;
    p1 = v2.z*(v1.y - v0.y) - v2.y*(v1.z - v0.z);
    r = e1 * abs(f0.z) + e2 * abs(f0.y);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a01 = (0, -f1z, f1y)
    r = e1*abs(f1.z) + e2*abs(f1.y);
    p0 = v0.z*(v2.y-v1.y) - v0.y*(v2.z-v1.z);
    p1 = v1.z*v2.y - v1.y*v2.z;
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a02 = (0, -f2z, f2y)
    r = e1*abs(f2.z) + e2*abs(f2.y);
    p0 = v0.y*v2.z - v0.z*v2.y;
    p1 = -v1.y*(v0.z-v2.z) + v1.z*(v0.y-v2.y);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a10 = (f0z, 0, -f0x)
    r = e0*abs(f0.z) + e2*abs(f0.x);
    p0 = v0.x*v1.z - v0.z*v1.x;
    p1 = v2.x*(v1.z-v0.z) - v2.z*(v1.x-v0.x);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a11 = (f1z, 0, -f1x)
    r = e0*abs(f1.z) + e2*abs(f1.x);
    p0 = v0.x*(v2.z-v1.z) - v0.z*(v2.x-v1.x);
    p1 = v1.x*v2.z - v1.z*v2.x;
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a12 = (f2z, 0, -f2x)
    r = e0*abs(f2.z) + e2*abs(f2.x);
    p0 = v0.z*v2.x - v0.x*v2.z;
    p1 = v1.x*(v0.z-v2.z) - v1.z*(v0.x-v2.x);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a20 = (-f0y, f0x, 0)
    r = e0*abs(f0.y) + e1*abs(f0.x);
    p0 = -v0.x*v1.y + v0.y*v1.x;
    p1 = -v2.x*(v1.y-v0.y) + v2.y*(v1.x-v0.x);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a21 = (-f1y, f1x, 0)
    r = e0*abs(f1.y) + e1*abs(f1.x);
    p0 = -v0.x*(v2.y-v1.y) + v0.y*(v2.x-v1.x);
    p1 = v2.x*v1.y - v2.y*v1.x;
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;

    // a22 = (-f2y, f2x, 0)
    r = e0*abs(f2.y) + e1*abs(f2.x);
    p0 = v0.x*v2.y - v0.y*v2.x;
    p1 = -v1.x*(v0.y-v2.y) + v1.y*(v0.x-v2.x);
    if (max(-max(p0, p1), min(p0, p1)) > r) return 0;


    // test three aces corresponding to the face normals of the AABB (category 1)
    if (max3(v0.x, v1.x, v2.x) < -e0 || min3(v0.x, v1.x, v2.x) > e0) return 0;
    if (max3(v0.y, v1.y, v2.y) < -e1 || min3(v0.y, v1.y, v2.y) > e1) return 0;
    if (max3(v0.z, v1.z, v2.z) < -e2 || min3(v0.z, v1.z, v2.z) > e2) return 0;

    // test separating axis corresponding to triangle face normal (category 2)
    laml::Vec3 plane_normal = laml::cross(f0, f1);
    real32 plane_dist = laml::dot(plane_normal, v0);

    r = e0*abs(plane_normal.x) + e1*abs(plane_normal.y) + e2*abs(plane_normal.z);
    real32 s = laml::dot(plane_normal, box_center) - plane_dist;
    return abs(s) <= r;
}
#endif
#endif

laml::Vec3 ClosestPointOnLineSegment(laml::Vec3 A, laml::Vec3 B, laml::Vec3 point) {
    laml::Vec3 AB = B - A;
    real32 t = laml::dot(point - A, AB) / laml::dot(AB, AB);
    t = min(max(t, 0), 1);
    return A + t*AB;
}

laml::Vec3 ClosestPointOnTriangle(laml::Vec3 point, laml::Vec3 p0, laml::Vec3 p1, laml::Vec3 p2) {
    // Determine whether line_plane_intersection is inside all triangle edges: 
    laml::Vec3 c0 = laml::cross(point - p0, p1 - p0);
    laml::Vec3 c1 = laml::cross(point - p1, p2 - p1);
    laml::Vec3 c2 = laml::cross(point - p2, p0 - p2);
    laml::Vec3 N = laml::normalize(laml::cross(p1 - p0, p2 - p0));
    bool inside = (laml::dot(c0, N) <= 0) && (laml::dot(c1, N) <= 0) && (laml::dot(c2, N) <= 0);
 
    if(inside) {
        return point;
    } else {
        laml::Vec3 result;

        // Edge 1:
        laml::Vec3 point1 = ClosestPointOnLineSegment(p0, p1, point);
        laml::Vec3 v1 = point - point1;
        float distsq = laml::dot(v1, v1);
        float best_dist = distsq;
        result = point1;
 
        // Edge 2:
        laml::Vec3 point2 = ClosestPointOnLineSegment(p1, p2, point);
        laml::Vec3 v2 = point - point2;
        distsq = laml::dot(v2, v2);
        if(distsq < best_dist) {
            result = point2;
            best_dist = distsq;
        }
 
        // Edge 3:
        laml::Vec3 point3 = ClosestPointOnLineSegment(p2, p0, point);
        laml::Vec3 v3 = point - point3;
        distsq = laml::dot(v3, v3);
        if(distsq < best_dist) {
            result = point3;
            best_dist = distsq;
        }

        return result;
    }
}

bool32 triangle_sphere_intersect(laml::Vec3 p0, laml::Vec3 p1, laml::Vec3 p2, laml::Vec3 sphere_center, real32 sphere_radius) {
    laml::Vec3 N = laml::normalize(laml::cross(p1 - p0, p2 - p0)); // plane normal
    real32 dist = laml::dot(sphere_center - p0, N); // signed distance between sphere and plane
    //if(!mesh.is_double_sided() && dist > 0)
    //    continue; // can pass through back side of triangle (optional)
    //if(dist < -radius || dist > radius)
    //    continue; // no intersection
    if (dist < -sphere_radius || dist > sphere_radius) {
        return false;
    }

    laml::Vec3 point0 = sphere_center - N * dist; // projected sphere center on triangle plane
 
    // Now determine whether point0 is inside all triangle edges: 
    laml::Vec3 c0 = laml::cross(point0 - p0, p1 - p0);
    laml::Vec3 c1 = laml::cross(point0 - p1, p2 - p1);
    laml::Vec3 c2 = laml::cross(point0 - p2, p0 - p2);
    bool inside = laml::dot(c0, N) <= 0 && laml::dot(c1, N) <= 0 && laml::dot(c2, N) <= 0;

    real32 radiussq = sphere_radius * sphere_radius; // sphere radius squared
 
    // Edge 1:
    laml::Vec3 point1 = ClosestPointOnLineSegment(p0, p1, sphere_center);
    laml::Vec3 v1 = sphere_center - point1;
    real32 distsq1 = laml::dot(v1, v1);
    bool intersects = distsq1 < radiussq;
 
    // Edge 2:
    laml::Vec3 point2 = ClosestPointOnLineSegment(p1, p2, sphere_center);
    laml::Vec3 v2 = sphere_center - point2;
    real32 distsq2 = laml::dot(v2, v2);
    intersects |= distsq2 < radiussq;
 
    // Edge 3:
    laml::Vec3 point3 = ClosestPointOnLineSegment(p2, p0, sphere_center);
    laml::Vec3 v3 = sphere_center - point3;
    real32 distsq3 = laml::dot(v3, v3);
    intersects |= distsq3 < radiussq;

    if(inside || intersects) {
        laml::Vec3 best_point = point0;
        laml::Vec3 intersection_vec;
 
        if(inside) {
            intersection_vec = sphere_center - point0;
        } else {
            laml::Vec3 d = sphere_center - point1;
            real32 best_distsq = laml::dot(d, d);
            best_point = point1;
            intersection_vec = d;
 
            d = sphere_center - point2;
            real32 distsq = laml::dot(d, d);
            if(distsq < best_distsq) {
                distsq = best_distsq;
                best_point = point2;
                intersection_vec = d;
            }
 
            d = sphere_center - point3;
            distsq = laml::dot(d, d);
            if(distsq < best_distsq) {
                distsq = best_distsq;
                best_point = point3; 
                intersection_vec = d;
            }
        }

        real32 len = laml::length(intersection_vec);
        laml::Vec3 penetration_normal = intersection_vec / len;
        real32 penetration_depth = sphere_radius - len;
        return true;
    }

    return false;
}

// line segment pq tested against triangle abc.
// returns if they are intersecting. If true, uvw is the barycentric coordinates of the intersection, and
// t is the parameter along the line segment at intersection with the plane
bool32 segment_intersect_triangle(laml::Vec3 p, laml::Vec3 q, laml::Vec3 a, laml::Vec3 b, laml::Vec3 c,
                                         real32& u, real32& v, real32& w, real32& t) {
    laml::Vec3 ab = b - a;
    laml::Vec3 ac = c - a;
    laml::Vec3 qp = p - q;

    // compute triangle normal
    laml::Vec3 n = laml::cross(ab, ac);

    // compute denominator d. if d <= 0, segment is parallel to or points away from the triangle, so exit early.
    real32 d = laml::dot(qp, n);
    if (abs(d) <= laml::eps<real32>) {
        // parallel
        return false;
    }
    if (d < 0.0f) {
        n = -n;
        d = -d;
        qp = -qp;
    }

    // compute intesection t value of pq with plane of triangle. segment intersects if 0 <= t <= 1.
    laml::Vec3 ap = p - a;
    t = laml::dot(ap, n);
    if (t < 0.0f) return false;
    if (t > d) return false; // remove htis line if you want a ray test (instaed of segment)

    // compute barycentric coords of intersect point, to see if within triangle
    laml::Vec3 e = laml::cross(qp, ap);
    v = laml::dot(ac, e);
    if (v < 0.0f || v > d) return false;
    w = -laml::dot(ab, e);
    if (w < 0.0f || (v + w) > d) return false;

    // segment/ray intersect triangle. perform delayed division
    real32 ood = 1.0f / d;
    t *= ood;
    v *= ood;
    w *= ood;
    u = 1.0f - v - w;
    return true;
}

// line segment ab tested against cylinder pq with radius r.
// returns if they are intersecting, and at what point t along the segment.
bool32 segment_intersect_cylinder(laml::Vec3 sa, laml::Vec3 sb, laml::Vec3 p, laml::Vec3 q, real32 r,
                                         real32& t) {
    laml::Vec3 d = q-p, m = sa-p, n = sb-sa;
    real32 md = laml::dot(m, d);
    real32 nd = laml::dot(n, d);
    real32 dd = laml::dot(d, d);

    // test if segment fully outside either endcap of cylinder
    if (md < 0.0f && md + nd < 0.0f) return false; // segment outside of 'p' side of cylinder
    if (md > dd && md + nd < dd) return false;     // segment outside of 'a' side of cylinder

    real32 nn = laml::dot(n, n);
    real32 mn = laml::dot(m, n);
    real32 a = dd * nn - nd*nd;
    real32 k = laml::dot(m, m) - r*r;
    real32 c = dd * k - md*md;

    if (abs(a) < laml::eps<real32>) {
        // segment runs parallel to cylinder axis
        if (c > 0.0f) return false; // 'a' and hus the segment lie outside cylinder
        // now known that segment intersects cylinderl figure out how it intersects
        if (md < 0.0f) t = -mn / nn;
        else if (md > dd) t = (nd - mn) / nn;
        else t = 0.0f;
        return true;
    }

    real32 b = dd*mn - nd*md;
    real32 discr = b*b - a*c;
    if (discr < 0.0f) return false; // no real roots, no intersection

    // NOTE: adding this to account for a weird case
    if (c < 0 && discr > laml::eps<real32>) {
        // A is inside the cylinder, so we know it intersects.
        real32 t1 = (-b - sqrt(discr)) / a;
        real32 t2 = (-b + sqrt(discr)) / a;

        if (t1 < 0.0f || t1 > 1.0f) {
            // check if other soln is valid
            if (t2 < 0.0f || t2 > 1.0f) {
                return false;
            } else {
                t = t2;
            }
        } else {
            // t1 is valid
            t = t1;
        }
    } else {
        t = (-b - sqrt(discr)) / a;
    }
    //

    //t = (-b - sqrt(discr)) / a;
    if (t < 0.0f || t > 1.0f) return false; // intersection lies outside segment
    if (md + t * nd < 0.0f) {
        // intersection outside of cylinder on 'p' side
        if (nd < 0.0f) return false; // segment pointing away from endcap
        t = -md / nd;
        // keep intersection if Dot(S(T) - p, S(t) - p) <= r^2
        return k + 2*t*(mn + t * nn) <= 0.0f;
    } else if (md + t*nd > dd) {
        // inteerscetion outside of cylinder on 'q' side
        if (nd >= 0.0f) return false;
        t = (dd - md) / nd;
        return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
    }

    // segment intersects cyylinder between endcaps; t is correct
    return true;
}

#if 0
static bool32 ray_intersect_cylinder(laml::Vec3 p, laml::Vec3 d, laml::Vec3 sphere_center, real32 radius,
                                     real32& t) {
    laml::Vec3 m = p - sphere_center;
    real32 b = laml::dot(m, d);
    real32 c = laml::dot(m, m) - radius*radius;
    // exit if r's origin oustide s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f) return false;
    real32 discr = b*b - c;
    if (discr < 0.0f) return false;
    t = -b - sqrt(discr);
    if (t < 0.0f) t = 0.0f;
    return true;
}
#endif

bool32 segment_intersect_cylinder(laml::Vec3 p, laml::Vec3 q, laml::Vec3 sphere_center, real32 radius,
                                         real32& t) {
    laml::Vec3 d = q - p;
    real32 segment_length = laml::length(d);
    d = d / segment_length;
    laml::Vec3 m = p - sphere_center;
    real32 b = laml::dot(m, d);
    real32 c = laml::dot(m, m) - radius*radius;
    // exit if r's origin oustide s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f) return false;
    real32 discr = b*b - c;
    if (discr < 0.0f) return false;
    t = -b - sqrt(discr);
    if (t < 0.0f) t = 0.0f;
    return (t <= segment_length);
}

bool32 triangle_capsule_intersect(collision_triangle triangle, collision_capsule capsule, laml::Vec3& contact_point) {
#if 1
    // turn capsule into a line segment
    laml::Vec3 A = capsule.A;// + capsule_position;
    laml::Vec3 B = capsule.B;// + capsule_position;
    // turn triangle into the minkowski sum of triangle and sphere
    //  - points become spheres
          /*(triangle.v1, capsule.radius)*/
          /*(triangle.v2, capsule.radius)*/
          /*(triangle.v3, capsule.radius)*/
    //  - edges become cylinders
          /*(triangle.v1, triangle.v2, capsule.radius)*/
          /*(triangle.v2, triangle.v3, capsule.radius)*/
          /*(triangle.v3, triangle.v1, capsule.radius)*/
    //  - face becomes two faces, moved outwards by radius along normal
    laml::Vec3 edge12_norm = laml::normalize(triangle.v2 - triangle.v1);
    laml::Vec3 edge13_norm = laml::normalize(triangle.v3 - triangle.v1);
    laml::Vec3 triangle_normal = laml::normalize(laml::cross(edge12_norm, edge13_norm));
    laml::Vec3 triangle_to_ray = laml::normalize(A - triangle.v1);
    real32 offset = (laml::dot(triangle_to_ray, triangle_normal) > 0.0f) ? capsule.radius : -capsule.radius;
    collision_triangle closest_face;
    closest_face.v1 = triangle.v1 + (offset*triangle_normal);
    closest_face.v2 = triangle.v2 + (offset*triangle_normal);
    closest_face.v3 = triangle.v3 + (offset*triangle_normal);

    // capsule/sphere intersection can be done in 3 parts then:
    // 1) test line segment against closest face to segment (what if the capsule straddles the shape?)
    {
        real32 u, v, w, t;
        if (segment_intersect_triangle(A, B, closest_face.v1, closest_face.v2, closest_face.v3, u, v, w, t)) {
            // intersection!
            contact_point = A + (B-A)*t;
            return true;
        } // otherwise, continue on...
    }
    // 2) test line segment against each cylinder
    {
        real32 t;
        if (segment_intersect_cylinder(A, B, triangle.v1, triangle.v2, capsule.radius, t)) {
            // intersection with edge 12!
            contact_point = A + (B-A)*t;
            return true;
        } else if (segment_intersect_cylinder(A, B, triangle.v2, triangle.v3, capsule.radius, t)) {
            // intersection with edge 23!
            contact_point = A + (B-A)*t;
            return true;
        } else if (segment_intersect_cylinder(A, B, triangle.v3, triangle.v1, capsule.radius, t)) {
            // intersection with edge 31!
            contact_point = A + (B-A)*t;
            return true;
        } // otherwise, continue on...
    }
    // 3) test line segment against each sphere
    {
        real32 t;
        if (segment_intersect_cylinder(A, B, triangle.v1, capsule.radius, t)) {
            // intersection with vertex 1!
            contact_point = A + (B-A)*t;
            return true;
        } else if (segment_intersect_cylinder(A, B, triangle.v2, capsule.radius, t)) {
            // intersection with vertex 2!
            contact_point = A + (B-A)*t;
            return true;
        } else if (segment_intersect_cylinder(A, B, triangle.v3, capsule.radius, t)) {
            // intersection with vertex 3!
            contact_point = A + (B-A)*t;
            return true;
        }
    }
    // if no intersection yet, shapes are not touching
    return false;

#else
    laml::Vec3 capsule_normal = laml::normalize(capsule.B - capsule.A);

    laml::Vec3 A = capsule.A + capsule_position;
    laml::Vec3 B = capsule.B + capsule_position;

    // triangle/ray-plane intersection
    // N is the triangle plane normal
    laml::Vec3 N = laml::normalize(laml::cross(triangle.v2 - triangle.v1, triangle.v3 - triangle.v1));
    real32 t = laml::dot(N, (triangle.v1 - A) / abs(laml::dot(N, capsule_normal)));
    laml::Vec3 line_plane_intersection = A + capsule_normal*t;

    // find closest triangle point to reference point
    laml::Vec3 reference_point;
    if (abs(laml::dot(capsule_normal, N)) < laml::eps<real32>) {
        //reference_point = ClosestPointOnTriangle(A, triangle.v1, triangle.v2, triangle.v3);

        laml::Vec3 sphere_center = B; //ClosestPointOnLineSegment(A, B, reference_point);

        return triangle_sphere_intersect(triangle.v1, triangle.v2, triangle.v3, sphere_center, capsule.radius);
    } else {
        reference_point = ClosestPointOnTriangle(line_plane_intersection, triangle.v1, triangle.v2, triangle.v3);

        laml::Vec3 sphere_center = ClosestPointOnLineSegment(A, B, reference_point);

        return triangle_sphere_intersect(triangle.v1, triangle.v2, triangle.v3, sphere_center, capsule.radius);
    }
#endif
}

bool32 triangle_sphere_intersect(collision_triangle triangle, collision_sphere sphere, laml::Vec3& contact_point) {
    if (triangle_sphere_intersect(triangle.v1, triangle.v2, triangle.v3, sphere.C, sphere.radius)) {
        contact_point = sphere.C;
        return true;
    }

    return false;
}


inline real32 squareDistance(const laml::Vec3& p0, const laml::Vec3& dir, real32 t, const laml::Vec3& point) {
    laml::Vec3 diff = point - p0;
    real32 fT = laml::dot(diff, dir);
    fT = max(fT, 0.0f);
    fT = min(fT, t);
    diff = diff - fT*dir;
    return laml::length_sq(diff);
}

// quick triangle culling for sphere-based sweeps
inline bool32 coarseCullingTri(const laml::Vec3& center, const laml::Vec3& dir, real32 t, real32 radius, const laml::Vec3* triVerts) {
    const laml::Vec3 triCenter = (triVerts[0] + triVerts[1] + triVerts[2]) * (1.0f/3.0f);

    // distance between the triangle center and the swept path (an LSS)
    real32 d = sqrt(squareDistance(center, dir, t, triCenter)) - radius - 0.0001f;

    if (d < 0.0f) // The triangle center lies inside the swept sphere
        return true;

    d*=d;

    // coarse capsule-vs-triangle overlap test ### distances could be precomputed?
    if(d <= laml::length_sq(triCenter-triVerts[0]))
        return true;
    if(d <= laml::length_sq(triCenter-triVerts[1]))
        return true;
    if(d <= laml::length_sq(triCenter-triVerts[2]))
        return true;

    return false;
}

// quick triangle rejection for sphere-based sweeps.
inline bool32 cullTriangle(const laml::Vec3* triVerts, const laml::Vec3& dir, real32 radius, real32 t, const real32 dpc0) {
    // PT: project triangle on axis
    const real32 dp0 = laml::dot(triVerts[0], dir);
    const real32 dp1 = laml::dot(triVerts[1], dir);
    const real32 dp2 = laml::dot(triVerts[2], dir);

    // PT: keep min value = earliest possible impact distance
    real32 dp = dp0;
    dp = min(dp, dp1);
    dp = min(dp, dp2);

    // PT: make sure we keep triangles that are about as close as best current distance
    radius += 0.002f;

    // PT: if earliest possible impact distance for this triangle is already larger than
    // sphere's current best known impact distance, we can skip the triangle
    if(dp>dpc0 + t + radius)
    {
        //PX_ASSERT(resx == 0.0f);
        return false;
    }

    // PT: if triangle is fully located before the sphere's initial position, skip it too
    const real32 dpc1 = dpc0 - radius;
    if(dp0<dpc1 && dp1<dpc1 && dp2<dpc1)
    {
        //PX_ASSERT(resx == 0.0f);
        return false;
    }

    //PX_ASSERT(resx != 0.0f);
    return true;
}

// combined triangle culling for sphere-based sweeps
inline bool32 rejectTriangle(const laml::Vec3& center, const laml::Vec3& unitDir, real32 curT, real32 radius, const laml::Vec3* triVerts, const real32 dpc0) {
    if(!coarseCullingTri(center, unitDir, curT, radius, triVerts))
        return true;
    if(!cullTriangle(triVerts, unitDir, radius, curT, dpc0))
        return true;
    return false;
}

inline laml::Vec3 ClosestPointOnTriangle2(const laml::Vec3& p, const laml::Vec3& a, const laml::Vec3& b, const laml::Vec3& c, const laml::Vec3& ab, const laml::Vec3& ac) {
    // Check if P in vertex region outside A
    //const laml::Vec3 ab = b - a;
    //const laml::Vec3 ac = c - a;
    const laml::Vec3 ap = p - a;
    const float d1 = laml::dot(ab, ap);
    const float d2 = laml::dot(ac, ap);
    if(d1<=0.0f && d2<=0.0f)
        return a;	// Barycentric coords 1,0,0

    // Check if P in vertex region outside B
    const laml::Vec3 bp = p - b;
    const float d3 = laml::dot(ab, bp);
    const float d4 = laml::dot(ac, bp);
    if(d3>=0.0f && d4<=d3)
        return b;	// Barycentric coords 0,1,0

    // Check if P in edge region of AB, if so return projection of P onto AB
    const float vc = d1*d4 - d3*d2;
    if(vc<=0.0f && d1>=0.0f && d3<=0.0f) {
        const float v = d1 / (d1 - d3);
        return a + v * ab;	// barycentric coords (1-v, v, 0)
    }

    // Check if P in vertex region outside C
    const laml::Vec3 cp = p - c;
    const float d5 = laml::dot(ab, cp);
    const float d6 = laml::dot(ac, cp);
    if(d6>=0.0f && d5<=d6)
        return c;	// Barycentric coords 0,0,1

    // Check if P in edge region of AC, if so return projection of P onto AC
    const float vb = d5*d2 - d1*d6;
    if(vb<=0.0f && d2>=0.0f && d6<=0.0f) {
        const float w = d2 / (d2 - d6);
        return a + w * ac;	// barycentric coords (1-w, 0, w)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    const float va = d3*d6 - d5*d4;
    if(va<=0.0f && (d4-d3)>=0.0f && (d5-d6)>=0.0f) {
        const float w = (d4-d3) / ((d4 - d3) + (d5-d6));
        return b + w * (c-b);	// barycentric coords (0, 1-w, w)
    }

    // P inside face region. Compute Q through its barycentric coords (u,v,w)
    const float denom = 1.0f / (va + vb + vc);
    const float v = vb * denom;
    const float w = vc * denom;
    return a + ab*v + ac*w;
}

#define LOCAL_EPSILON 0.00001f

// special version computing (u,v) even when the ray misses the tri. Version working on precomputed edges.
static inline uint32 rayTriSpecial(const laml::Vec3& orig, const laml::Vec3& dir, 
                                   const laml::Vec3& vert0, const laml::Vec3& edge1, const laml::Vec3& edge2, 
                                   real32& t, real32& u, real32& v)
{
    // Begin calculating determinant - also used to calculate U parameter
    const laml::Vec3 pvec = laml::cross(dir, edge2);

    // If determinant is near zero, ray lies in plane of triangle
    const real32 det = laml::dot(edge1, pvec);

    // the non-culling branch
    // if(det>-GU_CULLING_EPSILON_RAY_TRIANGLE && det<GU_CULLING_EPSILON_RAY_TRIANGLE)
    if(det>-LOCAL_EPSILON && det<LOCAL_EPSILON)
        return 0;
    const real32 oneOverDet = 1.0f / det;

    // Calculate distance from vert0 to ray origin
    const laml::Vec3 tvec = orig - vert0;

    // Calculate U parameter
    u = (laml::dot(tvec,pvec)) * oneOverDet;

    // prepare to test V parameter
    const laml::Vec3 qvec = laml::cross(tvec, edge1);

    // Calculate V parameter
    v = laml::dot(dir,qvec) * oneOverDet;

    if(u<0.0f || u>1.0f)
        return 1;
    if(v<0.0f || u+v>1.0f)
        return 1;

    // Calculate t, ray intersects triangle
    t = laml::dot(edge2, qvec) * oneOverDet;

    return 2;
}

// dir = p1 - p0
inline real32 distancePointSegmentSquaredInternal(const laml::Vec3& p0, const laml::Vec3& dir, const laml::Vec3& point, real32* param=NULL)
{
    laml::Vec3 diff = point - p0;
    real32 fT = laml::dot(diff, dir);

    if(fT<=0.0f) {
        fT = 0.0f;
    } else {
        const real32 sqrLen = laml::length_sq(dir);
        if(fT>=sqrLen) {
            fT = 1.0f;
            diff = diff - dir;
        } else {
            fT /= sqrLen;
            diff = diff - fT*dir;
        }
    }

    if(param)
        *param = fT;

    return laml::length_sq(diff);
}

static bool intersectRaySphere(const laml::Vec3& rayOrigin, const laml::Vec3& rayDir, const laml::Vec3& sphereCenter, float radius2, float& tmin, float& tmax)
{
    const laml::Vec3 CO = rayOrigin - sphereCenter;

    const float a = laml::dot(rayDir, rayDir);
    const float b = 2.0f * laml::dot(CO, rayDir);
    const float c = laml::dot(CO, CO) - radius2;

    const float discriminant = b * b - 4.0f * a * c;
    if(discriminant < 0.0f)
        return false;

    const float OneOver2A = 1.0f / (2.0f * a);
    const float sqrtDet = sqrtf(discriminant);
    tmin = (-b - sqrtDet) * OneOver2A;
    tmax = (-b + sqrtDet) * OneOver2A;
    if(tmin > tmax) {
        float tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    return true;
}

uint32 intersectRayCapsuleInternal(const laml::Vec3& rayOrigin, const laml::Vec3& rayDir, 
                                   const laml::Vec3& capsuleP0, const laml::Vec3& capsuleP1, 
                                   float radius, real32 s[2]) {
    const float radius2 = radius * radius;

    const laml::Vec3 AB = capsuleP1 - capsuleP0;
    const laml::Vec3 AO = rayOrigin - capsuleP0;

    const float AB_dot_d = laml::dot(AB, rayDir);
    const float AB_dot_AO = laml::dot(AB, AO);
    const float AB_dot_AB = laml::dot(AB, AB);
	
    const float OneOverABDotAB = AB_dot_AB!=0.0f ? 1.0f / AB_dot_AB : 0.0f;
    const float m = AB_dot_d * OneOverABDotAB;
    const float n = AB_dot_AO * OneOverABDotAB;

    const laml::Vec3 Q = rayDir - (AB * m);
    const laml::Vec3 R = AO - (AB * n);

    const float a = laml::dot(Q,Q);
    const float b = 2.0f * laml::dot(Q,R);
    const float c = laml::dot(R,R) - radius2;

    if(a == 0.0f)
    {
        float atmin, atmax, btmin, btmax;
        if(   !intersectRaySphere(rayOrigin, rayDir, capsuleP0, radius2, atmin, atmax)
           || !intersectRaySphere(rayOrigin, rayDir, capsuleP1, radius2, btmin, btmax))
            return 0;

        s[0] = atmin < btmin ? atmin : btmin;
        return 1;
    }

    const float discriminant = b * b - 4.0f * a * c;
    if(discriminant < 0.0f)
        return 0;

    const float OneOver2A = 1.0f / (2.0f * a);
    const float sqrtDet = sqrtf(discriminant);

    float tmin = (-b - sqrtDet) * OneOver2A;
    float tmax = (-b + sqrtDet) * OneOver2A;
    if (tmin > tmax) {
        float tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    const float t_k1 = tmin * m + n;
    if(t_k1 < 0.0f)
    {
        float stmin, stmax;
        if(intersectRaySphere(rayOrigin, rayDir, capsuleP0, radius2, stmin, stmax))
            s[0] = stmin;
        else 
            return 0;
    }
    else if(t_k1 > 1.0f)
    {
        float stmin, stmax;
        if(intersectRaySphere(rayOrigin, rayDir, capsuleP1, radius2, stmin, stmax))
            s[0] = stmin;
        else 
            return 0;
    }
    else
        s[0] = tmin;
    return 1;
}

#define GU_RAY_SURFACE_OFFSET 10.0f
inline bool intersectRayCapsule(const laml::Vec3& origin, const laml::Vec3& dir, const laml::Vec3& p0, const laml::Vec3& p1, real32 radius, real32& t) {
    // PT: move ray origin close to capsule, to solve accuracy issues.
    // We compute the distance D between the ray origin and the capsule's segment.
    // Then E = D - radius = distance between the ray origin and the capsule.
    // We can move the origin freely along 'dir' up to E units before touching the capsule.
    real32 l = distancePointSegmentSquaredInternal(p0, p1 - p0, origin);
    l = sqrt(l) - radius;

    // PT: if this becomes negative or null, the ray starts inside the capsule and we can early exit
    if(l<=0.0f) {
        t = 0.0f;
        return true;
    }

    // PT: we remove an arbitrary GU_RAY_SURFACE_OFFSET units to E, to make sure we don't go close to the surface.
    // If we're moving in the direction of the capsule, the origin is now about GU_RAY_SURFACE_OFFSET units from it.
    // If we're moving away from the capsule, the ray won't hit the capsule anyway.
    // If l is smaller than GU_RAY_SURFACE_OFFSET we're close enough, accuracy is good, there is nothing to do.
    if(l>GU_RAY_SURFACE_OFFSET)
        l -= GU_RAY_SURFACE_OFFSET;
    else
        l = 0.0f;

    // PT: move origin closer to capsule and do the raycast
    real32 s[2];
    const uint32 nbHits = intersectRayCapsuleInternal(origin + l*dir, dir, p0, p1, radius, s);
    if(!nbHits)
        return false;

    // PT: keep closest hit only
    if(nbHits == 1)
        t = s[0];
    else
        t = (s[0] < s[1]) ? s[0] : s[1];

    // PT: fix distance (smaller than expected after moving ray close to capsule)
    t += l;
    return true;
}

// sweep sphere against single triangle
bool32 sweep_sphere_single_triangle(const collision_triangle& tri, const laml::Vec3& normal, 
                                    const laml::Vec3& center, real32 radius, 
                                    const laml::Vec3& dir, real32& impactDistance, 
                                    bool testInitialOverlap) {

    const laml::Vec3 edge10 = tri.v2 - tri.v1;
    const laml::Vec3 edge20 = tri.v3 - tri.v1;

    if (testInitialOverlap) {
        const laml::Vec3 cp = ClosestPointOnTriangle2(center, tri.v1, tri.v2, tri.v3, edge10, edge20);

        if (laml::length_sq(cp - center) <= radius*radius) {
            impactDistance = 0.0f;
            return true;
        }
    }

    real32 u, v;
    {
        laml::Vec3 R = normal * radius;
        if (laml::dot(dir, R) >= 0.0f)
            R = -R;

        // The first point of the sphere to hit the triangle plane is the point of the sphere nearest to
        // the triangle plane. Hence, we use center - (normal*radius) below.

        // PT: casting against the extruded triangle in direction R is the same as casting from a ray moved by -R
        real32 t;
        const uint32 r = rayTriSpecial(center-R, dir, tri.v1, edge10, edge20, t, u, v);

        if (!r)
            return false;
        if (r == 2) {
            if (t < 0.0f)
                return false;
            impactDistance = t;
            return true;
        }
    }

    bool testTwoEdges = false;
    uint32 e0,e1,e2=0;
    if(u<0.0f) {
        if(v<0.0f) {
            // 0 or 0-1 or 0-2
            testTwoEdges = true;
            e0 = 0;
            e1 = 1;
            e2 = 2;
        } else if(u+v>1.0f) {
            // 2 or 2-0 or 2-1
            testTwoEdges = true;
            e0 = 2;
            e1 = 0;
            e2 = 1;
        } else {
            // 0-2
            e0 = 0;
            e1 = 2;
        }
    } else {
        if(v<0.0f) {
            if(u+v>1.0f) {
                // 1 or 1-0 or 1-2
                testTwoEdges = true;
                e0 = 1;
                e1 = 0;
                e2 = 2;
            } else {
                // 0-1
                e0 = 0;
                e1 = 1;
            }
        } else {
            Assert(u+v>=1.0f); // Else hit triangle
            // 1-2
            e0 = 1;
            e1 = 2;
        }
    }

    bool hit = false;
    real32 t;
    const laml::Vec3* triVerts = &tri.v1;
    if(intersectRayCapsule(center, dir, triVerts[e0], triVerts[e1], radius, t) && t>=0.0f) {
        impactDistance = t;
        hit = true;
    }
    if(testTwoEdges && intersectRayCapsule(center, dir, triVerts[e0], triVerts[e2], radius, t) && t>=0.0f) {
        if(!hit || t<impactDistance) {
            impactDistance = t;
            hit = true;
        }
    }

    return hit;
}

// if the swept geometry is already intersecting
inline bool32 setInitialOverlapResults(sweep_result& hit, const laml::Vec3& unitDir, uint32 faceIndex) {
    // PT: please write these fields in the order they are listed in the struct.
    hit.face_index	= faceIndex;
    //hit.flags		= PxHitFlag::eNORMAL|PxHitFlag::eFACE_INDEX;
    hit.normal		= -unitDir;
    hit.distance	= 0.0f;
    return true;	// PT: true indicates a hit, saves some lines in calling code
}

#define GU_EPSILON_SAME_DISTANCE 1e-3f
inline bool32 keepTriangle(float triImpactDistance, float triAlignmentValue,
                           float bestImpactDistance, float bestAlignmentValue, float maxDistance) {
    // Reject triangle if further than the maxDistance
    if(triImpactDistance > maxDistance)
        return false;

    // If initial overlap happens, keep the triangle
    if(triImpactDistance == 0.0f)
        return true;

    // tris have "similar" impact distances if the difference is smaller than 2*distEpsilon
    float distEpsilon = GU_EPSILON_SAME_DISTANCE; // pick a farther hit within distEpsilon that is more opposing than the previous closest hit

    // PT: make it a relative epsilon to make sure it still works with large distances
    distEpsilon *= max(1.0f, max(triImpactDistance, bestImpactDistance));

    // If new distance is more than epsilon closer than old distance
    if(triImpactDistance < bestImpactDistance - distEpsilon)
        return true;

    // If new distance is no more than epsilon farther than oldDistance and "face is more opposing than previous"
    if(triImpactDistance < bestImpactDistance+distEpsilon && triAlignmentValue < bestAlignmentValue)
        return true;

    // If alignment value is the same, but the new triangle is closer than the best distance
    if(triAlignmentValue == bestAlignmentValue && triImpactDistance < bestImpactDistance)
        return true;

    return false;
}

inline bool32 keepTriangleBasic(float triImpactDistance, float bestImpactDistance, float maxDistance) {
    // Reject triangle if further than the maxDistance
    if(triImpactDistance > maxDistance)
        return false;

    // If initial overlap happens, keep the triangle
    if(triImpactDistance == 0.0f)
        return true;

    // If new distance is more than epsilon closer than old distance
    if(triImpactDistance < bestImpactDistance)
        return true;

    return false;
}

const uint32 INVALID_U32 = ~0U;

inline bool32 shouldFlipNormal(const laml::Vec3& normal, 
                               bool meshBothSides, bool isDoubleSided, 
                               const laml::Vec3& triangleNormal, const laml::Vec3& dir) {
    // PT: this function assumes that input normal is opposed to the ray/sweep direction. This is always
    // what we want except when we hit a single-sided back face with 'meshBothSides' enabled.

    if(!meshBothSides || isDoubleSided)
        return false;

    Assert(laml::dot(normal, dir) <= 0.0f); // PT: if this fails, the logic below cannot be applied
    return laml::dot(triangleNormal, dir) > 0.0f; // PT: true for back-facing hits
}

// Based on Christer Ericson's book
laml::Vec3 closestPtPointTriangle(const laml::Vec3& p, const laml::Vec3& a, const laml::Vec3& b, const laml::Vec3& c, float& s, float& t) {
    // Check if P in vertex region outside A
    const laml::Vec3 ab = b - a;
    const laml::Vec3 ac = c - a;
    const laml::Vec3 ap = p - a;
    const float d1 = laml::dot(ab, ap);
    const float d2 = laml::dot(ac, ap);
    if(d1<=0.0f && d2<=0.0f) {
        s = 0.0f;
        t = 0.0f;
        return a; // Barycentric coords 1,0,0
    }

    // Check if P in vertex region outside B
    const laml::Vec3 bp = p - b;
    const float d3 = laml::dot(ab, bp);
    const float d4 = laml::dot(ac, bp);
    if(d3>=0.0f && d4<=d3) {
        s = 1.0f;
        t = 0.0f;
        return b;	// Barycentric coords 0,1,0
    }

    // Check if P in edge region of AB, if so return projection of P onto AB
    const float vc = d1*d4 - d3*d2;
    if(vc<=0.0f && d1>=0.0f && d3<=0.0f) {
        const float v = d1 / (d1 - d3);
        s = v;
        t = 0.0f;
        return a + v * ab;	// barycentric coords (1-v, v, 0)
    }

    // Check if P in vertex region outside C
    const laml::Vec3 cp = p - c;
    const float d5 = laml::dot(ab, cp);
    const float d6 = laml::dot(ac, cp);
    if(d6>=0.0f && d5<=d6) {
        s = 0.0f;
        t = 1.0f;
        return c; // Barycentric coords 0,0,1
    }

    // Check if P in edge region of AC, if so return projection of P onto AC
    const float vb = d5*d2 - d1*d6;
    if(vb<=0.0f && d2>=0.0f && d6<=0.0f) {
        const float w = d2 / (d2 - d6);
        s = 0.0f;
        t = w;
        return a + w * ac;	// barycentric coords (1-w, 0, w)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    const float va = d3*d6 - d5*d4;
    if(va<=0.0f && (d4-d3)>=0.0f && (d5-d6)>=0.0f) {
        const float w = (d4-d3) / ((d4 - d3) + (d5-d6));
        s = 1.0f-w;
        t = w;
        return b + w * (c-b);	// barycentric coords (0, 1-w, w)
    }

    // P inside face region. Compute Q through its barycentric coords (u,v,w)
    const float denom = 1.0f / (va + vb + vc);
    const float v = vb * denom;
    const float w = vc * denom;
    s = v;
    t = w;
    return a + ab*v + ac*w;
}

void computeSphereTriImpactData(laml::Vec3& hit, laml::Vec3& normal, const laml::Vec3& center, 
                                const laml::Vec3& dir, float t, const collision_triangle& tri) {
    const laml::Vec3 newSphereCenter = center + dir*t;

    // We need the impact point, not computed by the new code
    real32 u, v;
    const laml::Vec3 localHit = closestPtPointTriangle(newSphereCenter, tri.v1, tri.v2, tri.v3, u, v);

    // This is responsible for the cap-vs-box stuck while jumping. However it's needed to slide on box corners!
    // PT: this one is also dubious since the sphere/capsule center can be far away from the hit point when the radius is big!
    laml::Vec3 localNormal = newSphereCenter - localHit;
    const real32 m = laml::length(localNormal);
    localNormal = localNormal / m;
    if(m<1e-3f) {
        localNormal = laml::cross(tri.v2-tri.v1, tri.v3-tri.v1);
    }

    hit = localHit;
    normal = localNormal;
}

// PT: computes proper impact data for sphere-sweep-vs-tri, after the closest tri has been found
inline bool32 computeSphereTriangleImpactData(sweep_result& h, laml::Vec3& triNormalOut, uint32 index, real32 curT, 
                                                     const laml::Vec3& center, const laml::Vec3& unitDir, const laml::Vec3& bestTriNormal,
                                                     const collision_triangle* triangles,
                                                     bool isDoubleSided, bool meshBothSides) {
    if(index==INVALID_U32)
        return false; // We didn't touch any triangle

    // Compute impact data only once, using best triangle
    laml::Vec3 hitPos, normal;
    computeSphereTriImpactData(hitPos, normal, center, unitDir, curT, triangles[index]);

    // PT: by design, returned normal is opposed to the sweep direction.
    if(shouldFlipNormal(normal, meshBothSides, isDoubleSided, bestTriNormal, unitDir))
        normal = -normal;

    h.position	= hitPos;
    h.normal	= normal;
    h.distance	= curT;
    h.face_index	= index;
    //h.flags		= PxHitFlag::eNORMAL|PxHitFlag::ePOSITION;
    triNormalOut = bestTriNormal;
    return true;
}

// move sphere through a list of triangles, and see how far it can go
bool32 sweep_sphere_triangles(uint32 num_tris, collision_triangle* triangles, laml::Vec3 sphere_center, real32 sphere_radius,
                              laml::Vec3 unit_direction, real32 distance,
                              sweep_result& hit_result, laml::Vec3& tri_normal_out) {
    if (num_tris == 0)
        return false;

    // TODO: make these flags?
    bool32 isDoubleSided = true; 
    bool32 meshBothSides = true;
    bool32 doBackfaceCulling = !isDoubleSided && !meshBothSides;
    bool32 testInitialOverlap = true;
    bool32 any_hit = false;

    uint32 index = INVALID_U32;

    real32 curr_t = distance;
    const real32 dpc0 = laml::dot(sphere_center, unit_direction);

    real32 best_align_value = 2.0f;
    laml::Vec3 best_tri_normal(0.0f);

    for(uint32 i = 0; i < num_tris; i++) {
        const collision_triangle& current_tri = triangles[i];

        if(rejectTriangle(sphere_center, unit_direction, curr_t, sphere_radius, &current_tri.v1, dpc0))
            continue;

        laml::Vec3 tri_normal = laml::cross(current_tri.v2 - current_tri.v1, current_tri.v3 - current_tri.v1);

        if (doBackfaceCulling && (laml::dot(tri_normal, unit_direction) > 0.0f))
            continue;

        const real32 magnitude = laml::length(tri_normal);
        if (magnitude == 0.0f) // TODO: check if this is valid
            continue;

        tri_normal = tri_normal / magnitude;
        
        real32 curr_dist;
        if (!sweep_sphere_single_triangle(current_tri, tri_normal, sphere_center, sphere_radius, unit_direction, curr_dist, testInitialOverlap))
            continue;

        const real32 hit_dot = -abs(laml::dot(tri_normal,unit_direction));
        if(keepTriangle(curr_dist, hit_dot, curr_t, best_align_value, distance)) {
            if(curr_dist==0.0f) {
                tri_normal_out = -unit_direction;
                return setInitialOverlapResults(hit_result, unit_direction, i);
            }

            curr_t = min(curr_t, curr_dist); // exact lower bound

            index = i;		
            best_align_value = hit_dot; 
            best_tri_normal = tri_normal;
            if(any_hit)
                break;
        } else if(keepTriangleBasic(curr_dist, curr_t, distance)) {
            curr_t = min(curr_t, curr_dist); // exact lower bound
        }
    }
    return computeSphereTriangleImpactData(hit_result, tri_normal_out, index, curr_t, 
                                           sphere_center, unit_direction, best_tri_normal, 
                                           triangles, isDoubleSided, meshBothSides);
}