#include "raytracer1c.h"
#include "raytracer1c_math.h"

#include <stdio.h>
#include <string.h>

struct Color shade_ray(int idx_closest_obj, struct PointVec surface_intersection_point, struct PointVec normal, int obj_type)
{
    if (idx_closest_obj == -1) {return params.bkgcolor;}

	struct MtlColor mtlcolor;

	if(obj_type < 2) //sphere
	{
    	mtlcolor = params.mtlcolors[params.spheres[idx_closest_obj].color_idx];
	}
	else //triangle
	{
		mtlcolor = params.mtlcolors[params.triangles[idx_closest_obj].color_idx];
	}
    struct Color final_color;

	struct Color od_bak;
	od_bak.r = mtlcolor.od.r;
	od_bak.g = mtlcolor.od.g;
	od_bak.b = mtlcolor.od.b;

	if(obj_type == SPHERE_TEXTURED)
	{
		double phi = acos(normal.z);
		double theta = atan2(normal.y, normal.x);
		double v = phi/PI;
		double u;
		if(theta > 0)
		{
			u = theta / (2 * PI);
		}
		else
		{
			u = (2 * PI + theta) / (2 * PI);
		}
		struct Texture texture = params.textures[params.spheres[idx_closest_obj].texture_idx];
		mtlcolor.od.r = texture.pixels[(int)(v * texture.height)][(int)(u * texture.width)].r;
		mtlcolor.od.g = texture.pixels[(int)(v * texture.height)][(int)(u * texture.width)].g;
		mtlcolor.od.b = texture.pixels[(int)(v * texture.height)][(int)(u * texture.width)].b;
	}

	if(obj_type == TRIANGLE_SMOOTH_TEXTURED || obj_type == TRIANGLE_UNSMOOTH_TEXTURED)
	{
		struct PointVec e1 = pvadd(params.vertices[params.triangles[idx_closest_obj].v2_idx], pvmult(params.vertices[params.triangles[idx_closest_obj].v1_idx], -1));
		struct PointVec e2 = pvadd(params.vertices[params.triangles[idx_closest_obj].v3_idx], pvmult(params.vertices[params.triangles[idx_closest_obj].v1_idx], -1));
		struct PointVec ep = pvadd(surface_intersection_point, pvmult(params.vertices[params.triangles[idx_closest_obj].v1_idx], -1));

		double d11 = pvdot(e1, e1);
		double d12 = pvdot(e1, e2);
		double d22 = pvdot(e2, e2);
		double d1p = pvdot(e1, ep);
		double d2p = pvdot(e2, ep);

		double det = (d11 * d22 - d12 * d12);

		double beta  = (d22 * d1p - d12 * d2p) / det;
		double gamma = (d11 * d2p - d12 * d1p) / det;
		double alpha = 1 - beta - gamma;

		double u = alpha * params.vtexts[params.triangles[idx_closest_obj].v1_text_idx].u + beta * params.vtexts[params.triangles[idx_closest_obj].v2_text_idx].u + gamma * params.vtexts[params.triangles[idx_closest_obj].v3_text_idx].u;
		double v = alpha * params.vtexts[params.triangles[idx_closest_obj].v1_text_idx].v + beta * params.vtexts[params.triangles[idx_closest_obj].v2_text_idx].v + gamma * params.vtexts[params.triangles[idx_closest_obj].v3_text_idx].v;

		struct Texture texture = params.textures[params.triangles[idx_closest_obj].texture_idx];
		double intpart = 0;

		int i = (int)(modf(v, &intpart) * (texture.height - 1));
		int j = (int)(modf(u, &intpart) * (texture.width - 1));

		mtlcolor.od.r = texture.pixels[i][j].r;
		mtlcolor.od.g = texture.pixels[i][j].g;
		mtlcolor.od.b = texture.pixels[i][j].b;
	}

    // Ambient component
    final_color.r = mtlcolor.ka * mtlcolor.od.r;
    final_color.g = mtlcolor.ka * mtlcolor.od.g;
    final_color.b = mtlcolor.ka * mtlcolor.od.b;

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
                if ((j != idx_closest_obj || obj_type > 1) && (t >= 0 && (params.lights[i].type == 0 || t <= pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))))))
                {
					if(1)
					{
						printf("set shadow flag\n");
					}
                    shadow_flag = 1;
                }
            }
        }

		for (int j = 0; j < params.numtriangles && !shadow_flag; j++)
		{
			struct PointVec e_1 = pvadd(params.vertices[params.triangles[j].v2_idx], pvmult(params.vertices[params.triangles[j].v1_idx], -1));
			struct PointVec e_2 = pvadd(params.vertices[params.triangles[j].v3_idx], pvmult(params.vertices[params.triangles[j].v1_idx], -1));
			struct PointVec n = pvnormalize(pvcross(e_1, e_2));

			double D = -(n.x * params.vertices[params.triangles[j].v1_idx].x + n.y * params.vertices[params.triangles[j].v1_idx].y + n.z * params.vertices[params.triangles[j].v1_idx].z);
			
			if(debug5)
			{
				printf("checking triangle %d\n", i);
				printf("n: %f, %f, %f\n", n.x, n.y, n.z);
				printf("D: %f\n", D);
			}

			if(n.x * ray_to_light.dir.x + n.y * ray_to_light.dir.y + n.z * ray_to_light.dir.z != 0)
			{
				double t = -(n.x * ray_to_light.loc.x + n.y * ray_to_light.loc.y + n.z * ray_to_light.loc.z + D)/(n.x * ray_to_light.dir.x + n.y * ray_to_light.dir.y + n.z * ray_to_light.dir.z);
				if((j != idx_closest_obj || obj_type < 2) && (t >= 0 && (params.lights[i].type == 0 || t <= pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))))))
				{
					struct PointVec plane_intersection_point = pvadd(ray_to_light.loc, pvmult(ray_to_light.dir, t));
					struct PointVec ep = pvadd(plane_intersection_point, pvmult(params.vertices[params.triangles[j].v1_idx], -1));

					if(debug5)
					{
						printf("t: %f\n", t);
						printf("found plane intersection point: %f %f %f\n", plane_intersection_point.x, plane_intersection_point.y, plane_intersection_point.z);
					}
					
					double d11 = pvdot(e_1, e_1);
					double d12 = pvdot(e_1, e_2);
					double d22 = pvdot(e_2, e_2);
					double d1p = pvdot(e_1, ep);
					double d2p = pvdot(e_2, ep);

					double det = (d11 * d22 - d12 * d12);

					if(det != 0)
					{
						double beta  = (d22 * d1p - d12 * d2p) / det;
						double gamma = (d11 * d2p - d12 * d1p) / det;
						if((beta + gamma) <= 1 && (beta + gamma) >= 0 && (beta >= 0) && (gamma >= 0) && (beta <= 1) && (gamma <= 1))
						{
							shadow_flag = 1;
						}
					}
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

	mtlcolor.od.r = od_bak.r;
	mtlcolor.od.g = od_bak.g;
	mtlcolor.od.b = od_bak.b;

    return final_color;
}



struct Color trace_ray(struct Ray ray)
{
	double dist_closest_obj = __DBL_MAX__;
	int idx_closest_obj = -1;
	int obj_type = -1;
	struct PointVec surface_intersection_point;
	struct PointVec normal;

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
				obj_type = params.spheres[i].type;
				surface_intersection_point = pvadd(ray.loc, pvmult(ray.dir, t));
				struct PointVec center_to_surface = pvadd(surface_intersection_point, pvmult(params.spheres[idx_closest_obj].c, -1));
    			normal = pvnormalize(center_to_surface);
			}
		}
	}

	for(int i = 0; i < params.numtriangles; i++)
	{
		struct PointVec e1 = pvadd(params.vertices[params.triangles[i].v2_idx], pvmult(params.vertices[params.triangles[i].v1_idx], -1));
		struct PointVec e2 = pvadd(params.vertices[params.triangles[i].v3_idx], pvmult(params.vertices[params.triangles[i].v1_idx], -1));
		struct PointVec n = pvnormalize(pvcross(e1, e2));

		double D = -(n.x * params.vertices[params.triangles[i].v1_idx].x + n.y * params.vertices[params.triangles[i].v1_idx].y + n.z * params.vertices[params.triangles[i].v1_idx].z);
		
		if(debug5)
		{
			printf("checking triangle %d\n", i);
			printf("n: %f, %f, %f\n", n.x, n.y, n.z);
			printf("D: %f\n", D);
		}

		if(n.x * ray.dir.x + n.y * ray.dir.y + n.z * ray.dir.z != 0)
		{
			double t = -(n.x * ray.loc.x + n.y * ray.loc.y + n.z * ray.loc.z + D)/(n.x * ray.dir.x + n.y * ray.dir.y + n.z * ray.dir.z);
			if(t >= 0 && t < dist_closest_obj)
			{
				struct PointVec plane_intersection_point = pvadd(ray.loc, pvmult(ray.dir, t));
				struct PointVec ep = pvadd(plane_intersection_point, pvmult(params.vertices[params.triangles[i].v1_idx], -1));

				if(debug5)
				{
					printf("t: %f\n", t);
					printf("found plane intersection point: %f %f %f\n", plane_intersection_point.x, plane_intersection_point.y, plane_intersection_point.z);
				}
				
				double d11 = pvdot(e1, e1);
				double d12 = pvdot(e1, e2);
				double d22 = pvdot(e2, e2);
				double d1p = pvdot(e1, ep);
				double d2p = pvdot(e2, ep);

				double det = (d11 * d22 - d12 * d12);

				if(det != 0)
				{
					double beta  = (d22 * d1p - d12 * d2p) / det;
					double gamma = (d11 * d2p - d12 * d1p) / det;
					// printf("beta: %f\n", beta);
					// printf("gamma: %f\n", gamma);
					if((beta + gamma) <= 1 && (beta + gamma) >= 0 && (beta >= 0) && (gamma >= 0) && (beta <= 1) && (gamma <= 1))
					{
						// printf("in for triangle %d", i);
						dist_closest_obj = t;
						idx_closest_obj = i;
						obj_type = params.triangles[i].type;
						surface_intersection_point = plane_intersection_point;
						// printf("type in trace_ray: %d\n", params.triangles[i].type);
						if(params.triangles[i].type == TRIANGLE_UNSMOOTH_TEXTURED || params.triangles[i].type == TRIANGLE_UNSMOOTH_UNTEXTURED) // non-smooth shading
						{
							struct PointVec e1 = pvadd(params.vertices[params.triangles[idx_closest_obj].v2_idx], pvmult(params.vertices[params.triangles[idx_closest_obj].v1_idx], -1));
							struct PointVec e2 = pvadd(params.vertices[params.triangles[idx_closest_obj].v3_idx], pvmult(params.vertices[params.triangles[idx_closest_obj].v1_idx], -1));
							normal = pvnormalize(pvcross(e1, e2));
						}
						else
						{
							normal = pvadd(pvmult(params.vnorms[params.triangles[i].v1_norm_idx], 1 - beta - gamma), 
									 pvadd(pvmult(params.vnorms[params.triangles[i].v2_norm_idx], beta), 
									 	   pvmult(params.vnorms[params.triangles[i].v3_norm_idx], gamma)));
							normal = pvnormalize(normal);
							// printf("tried to smooth\n");
						}
					}
				}
			}
		}
	}
	return shade_ray(idx_closest_obj, surface_intersection_point, normal, obj_type);
}