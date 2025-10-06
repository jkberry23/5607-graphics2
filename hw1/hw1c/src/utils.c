#include "raytracer1c.h"
#include "utils.h"

double pvmagnitude(struct PointVec vec)
{
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
}

struct PointVec pvnormalize(struct PointVec vec)
{
	struct PointVec rvec;
	double mag = sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
	rvec.x = vec.x / mag;
	rvec.y = vec.y / mag;
	rvec.z = vec.z / mag;
	return rvec;
}

struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2)
{
	struct PointVec rvec;
	rvec.x = vec1.y * vec2.z - vec1.z * vec2.y;
	rvec.y = vec1.z * vec2.x - vec1.x * vec2.z;
	rvec.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return rvec;
}

struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2)
{
	struct PointVec rvec;
	rvec.x = vec1.x + vec2.x;
	rvec.y = vec1.y + vec2.y;
	rvec.z = vec1.z + vec2.z;
	return rvec;
}

struct PointVec pvmult(struct PointVec vec, double n)
{
	struct PointVec rvec;
	rvec.x = vec.x * n;
	rvec.y = vec.y * n;
	rvec.z = vec.z * n;
	return rvec;
}

double pvdot(struct PointVec vec1, struct PointVec vec2)
{
	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
}

double smallest_positive(double d1, double d2)
{
	if(d1 >= 0 && d2 >= 0)
	{
		return fmin(d1, d2);
	}

	else if(d1 >= 0)
	{
		return d1;
	}

	else if(d2 >= 0){
		return d2;
	}
	else{
		return -1;
	}
}