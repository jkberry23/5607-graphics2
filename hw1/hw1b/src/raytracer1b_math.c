#include "raytracer1b.h"
#include "raytracer1b_math.h"

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

struct Color shade_ray(int idx_closest_obj, struct PointVec surface_intersection_point)
{
    if (idx_closest_obj == -1)
    {
        return params.bkgcolor;
    }

	if(debug4)
	{
		printf("found sphere\n");
	}

    struct MtlColor mtlcolor = params.mtlcolors[params.spheres[idx_closest_obj].color_idx];
    struct Color final_color;

    // Ambient component
    final_color.r = mtlcolor.ka * mtlcolor.od.r;
    final_color.g = mtlcolor.ka * mtlcolor.od.g;
    final_color.b = mtlcolor.ka * mtlcolor.od.b;

    struct PointVec center_to_surface = pvadd(surface_intersection_point, pvmult(params.spheres[idx_closest_obj].c, -1));
    struct PointVec normal = pvnormalize(center_to_surface);

    for (int i = 0; i < params.numlights; i++)
    {
        struct Ray ray_to_light;
        ray_to_light.loc = surface_intersection_point;

        if (params.lights[i].type == 1)
        {
            ray_to_light.dir = (pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1)));
			if(debug4)
			{
				printf("Light location:  %f, %f, %f\n", params.lights[i].locdir.x, params.lights[i].locdir.y, params.lights[i].locdir.z);
				printf("Surfc location: %f, %f, %f\n", surface_intersection_point.x, surface_intersection_point.y, surface_intersection_point.z);
				printf("Ray_to_light  :  %f, %f, %f\n", ray_to_light.dir.x, ray_to_light.dir.y, ray_to_light.dir.z);
			}
			ray_to_light.dir = pvnormalize(ray_to_light.dir);
			if(debug4)
			{
				printf("Ray_to_light_n:  %f, %f, %f\n", ray_to_light.dir.x, ray_to_light.dir.y, ray_to_light.dir.z);
			}
        }
        else // Directional light
        {
            ray_to_light.dir = pvnormalize(pvmult(params.lights[i].locdir, -1));
        }

        int shadow_flag = 0;

        // Determine whether the current light is shadowed
        for (int j = 0; j < params.numspheres && !shadow_flag; j++)
        {
            double B = 2 * (ray_to_light.dir.x * (ray_to_light.loc.x - params.spheres[j].c.x) +
                            ray_to_light.dir.y * (ray_to_light.loc.y - params.spheres[j].c.y) +
                            ray_to_light.dir.z * (ray_to_light.loc.z - params.spheres[j].c.z));

            double C = pow(ray_to_light.loc.x - params.spheres[j].c.x, 2) +
                       pow(ray_to_light.loc.y - params.spheres[j].c.y, 2) +
                       pow(ray_to_light.loc.z - params.spheres[j].c.z, 2) -
                       pow(params.spheres[j].r, 2);

            double disc = pow(B, 2) - 4 * C;

            if (disc >= 0)
            {
				if(debug4)
				{	
					printf("t1: %f\nt2: %f\n", (-B + sqrt(disc)) / 2, (-B - sqrt(disc)) / 2);
				}
                double t = smallest_positive((-B + sqrt(disc)) / 2, (-B - sqrt(disc)) / 2);
				if(debug4)
				{	
					printf("t: %f\n", t);
					printf("ray_to_light.loc: %f, %f, %f\n", ray_to_light.loc.x, ray_to_light.loc.y, ray_to_light.loc.z);
					printf("t to light: %f\n", pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))));
				}
                if (j != idx_closest_obj && (t >= 0 && (params.lights[i].type == 0 || t <= pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))))))
                {
					if(debug4)
					{
						printf("set shadow flag\n");
					}
                    shadow_flag = 1;
                }
            }
        }

		if(debug4)
		{
			printf("shadow flag: %s\n", shadow_flag ? "true" : "false");
		}

        if (shadow_flag == 0) // Not shadowed, get diffuse & specular components
        {
            struct PointVec l = ray_to_light.dir;
            struct PointVec v = pvnormalize(pvadd(params.eye, pvmult(surface_intersection_point, -1)));
            struct PointVec h = pvnormalize(pvadd(l, v));

            double nl = fmax(0.0, pvdot(normal, l));
            double nhn = pow(fmax(0.0, pvdot(normal, h)), mtlcolor.n);

            final_color.r += (mtlcolor.od.r * mtlcolor.kd * nl + mtlcolor.os.r * mtlcolor.ks * nhn) * params.lights[i].intensity;
            final_color.g += (mtlcolor.od.g * mtlcolor.kd * nl + mtlcolor.os.g * mtlcolor.ks * nhn) * params.lights[i].intensity;
            final_color.b += (mtlcolor.od.b * mtlcolor.kd * nl + mtlcolor.os.b * mtlcolor.ks * nhn) * params.lights[i].intensity;
        }
    }

    // Clamp colors
    double max_color = fmax(final_color.r, fmax(final_color.g, final_color.b));
    if (max_color > 1)
    {
        final_color.r /= max_color;
        final_color.g /= max_color;
        final_color.b /= max_color;
    }

    return final_color;
}



struct Color trace_ray(struct Ray ray)
{
	double dist_closest_obj = __DBL_MAX__;
	double idx_closest_obj = -1;
	struct PointVec surface_intersection_point;

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

		if(disc >= 0)
		{
			double t = smallest_positive((-B + sqrt(disc)) / 2, (-B - sqrt(disc)) / 2);
			if(t >= 0 && t < dist_closest_obj)
			{
				dist_closest_obj = t;
				idx_closest_obj = i;
				surface_intersection_point = pvadd(ray.loc, pvmult(ray.dir, t));
			}
		}
	}
	return shade_ray(idx_closest_obj, surface_intersection_point);
}