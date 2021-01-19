#ifndef __MY_PHYSICS__
#define __MY_PHYSICS__

#include "engine/utils/types_3d.h"

bool CalculateIntersectionPlaneSegment(const YVec3f& pDir1, const YVec3f& pDir2, const YVec3f& pPoint, const YVec3f& sDir, const YVec3f& sStart, const float& sLength, YVec3f& intersection)
{
	//ax + by + cz + d = 0
	//(a,b,c) represent the normal of the plane
	//x = Ax + (Bx - Ax) * t
	//y = Ay + (By - Ay) * t
	//z = Az + (Bz - Az) * t
	//(Ax, Ay, Az) is the start of the segment
	//B-A is the direction
	//t is the length

	YVec3f pNormal = pDir1.cross(pDir2);
	float d = -(pNormal.X * pPoint.X + pNormal.Y * pPoint.Y + pNormal.Z * pPoint.Z);
	float t;
	float numerator = -(pNormal.X * sStart.X + pNormal.Y * sStart.Y + pNormal.Z * sStart.Z + d);
	float denominator = pNormal.X * sDir.X + pNormal.Y * sDir.Y + pNormal.Z * sDir.Z;
	if (denominator != 0)
	{
		t = numerator / denominator;
		intersection = sStart + sDir * t;
		return abs(t) < sLength && t != 0;
	}
	return false;
}

bool IsPointWithinFace(const YVec3f& f1, const YVec3f& f2, const YVec3f& f3, const YVec3f& f4, const YVec3f& p)
{
	YVec3f f1f2 = f2 - f1;
	YVec3f f2f3 = f3 - f2;
	YVec3f f3f4 = f4 - f3;
	YVec3f f4f1 = f1 - f4;
	YVec3f f1p = p - f1;
	YVec3f f2p = p - f2;
	YVec3f f3p = p - f3;
	YVec3f f4p = p - f4;

	YVec3f v1 = f1f2.cross(f1p);
	YVec3f v2 = f2f3.cross(f2p);
	YVec3f v3 = f3f4.cross(f3p);
	YVec3f v4 = f4f1.cross(f4p);

	return v1.dot(v2) > 0 && v1.dot(v3) > 0 && v1.dot(v4) > 0 && v2.dot(v3) > 0 && v2.dot(v4) > 0 && v3.dot(v4) > 0;
}

#endif