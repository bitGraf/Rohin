#include "Collision.h"


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
#include "Engine/Memory/MemoryUtils.h"

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

bool32 triangle_cube_intersect(collision_triangle t, laml::Vec3 box_center, laml::Vec3 box_halfsize) {
    float triverts[3][3];
    memory_copy(triverts[0], t.v1._data, 3*sizeof(float));
    memory_copy(triverts[1], t.v2._data, 3*sizeof(float));
    memory_copy(triverts[2], t.v3._data, 3*sizeof(float));

    return triBoxOverlap(box_center._data, box_halfsize._data, triverts);
}
#endif