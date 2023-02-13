# Big-Disc-Mk-II
A second attempt at a game using lessons learned from big-time-disc-tomorrow

# collision stuff:
physx:
    bool GU::sweepCapsuleTriangles(...)
    bool Gu::sweepCapsuleTriangles_Precise(	

    capsule-triangle intesection test:   bool Gu::intersectCapsuleTriangle(

## notes:
if capsule is degenerate (is a sphere) or if the sweep direction is colinear with the capsule segment
    -> instead do a swept sphere test against the triangles.
       sweep along segment axis if colinear.
if already intersection:
    if(testInitialOverlap && intersectCapsuleTriangle(triNormal, currentTri.verts[0], currentTri.verts[1], currentTri.verts[2], capsule, params))
	{
		triNormalOut = -unitDir;
		return setInitialOverlapResults(hit, unitDir, i);
	}

    triangle_normal is opposite the direction of the sweep