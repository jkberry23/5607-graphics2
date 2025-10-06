#include "raytracer1a.h"
#include "raytracer1a_math.h"

#include <stdio.h>

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

struct Color shade_ray(int idx_closest_obj)
{
	if(idx_closest_obj == -1)
	{
		return params.bkgcolor;
	}
	return params.mtlcolors[params.spheres[idx_closest_obj].color_idx];
}

struct Color trace_ray(struct Ray ray)
{
	double dist_closest_obj = __DBL_MAX__;
	double idx_closest_obj = -1;

	if(debug3)
	{
		printf("ray: <%f, %f, %f> + t * <%f, %f, %f>\n", ray.loc.x, ray.loc.y, ray.loc.z, ray.dir.x, ray.dir.y, ray.dir.z);
		printf("magnitude of dir'n vector: %f\n", pvmagnitude(ray.dir));
	}

	for(int i = 0; i < params.numspheres; i++)
	{
		double B = 2 * (ray.dir.x * (ray.loc.x - params.spheres[i].c.x) +
						ray.dir.y * (ray.loc.y - params.spheres[i].c.y) +
						ray.dir.z * (ray.loc.z - params.spheres[i].c.z));

		double C = pow(ray.loc.x - params.spheres[i].c.x, 2) +
				   pow(ray.loc.y - params.spheres[i].c.y, 2) +
				   pow(ray.loc.z - params.spheres[i].c.z, 2) - 
				   pow(params.spheres[i].r, 2);

		double disc = pow(B,2) - 4*C;

		if(disc < 0) {}

		else if(disc == 0)
		{
			double t = -B/2;
			if(t >= 0 && t < dist_closest_obj)
			{
				dist_closest_obj = t;
				idx_closest_obj = i;
			}
		}

		else
		{
			double t = (-B + sqrt(disc)) / 2;
			if(t >= 0 && t < dist_closest_obj)
			{
				dist_closest_obj = t;
				idx_closest_obj = i;
			}
			t = (-B - sqrt(disc)) / 2;
			if(t >= 0 && t < dist_closest_obj)
			{
				dist_closest_obj = t;
				idx_closest_obj = i;
			}
		}
	}
	return shade_ray(idx_closest_obj);
}