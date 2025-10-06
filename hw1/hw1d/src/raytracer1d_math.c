#include "raytracer1d.h"
#include "raytracer1d_math.h"

#include <stdio.h>
#include <string.h>

struct Color shade_ray(int idx_closest_obj, struct Ray incident_ray, int enter_exit_flag, 
					   struct PointVec surface_intersection_point, struct PointVec sfc_outward_normal, 
					   int obj_type, int num_recursions, double ni)
{
	int debug = 0;
	if(debug)
	{
		printf("shade_ray called:\n");
		printf("\t%s OBJECT %d\n", enter_exit_flag == RAY_ENTERING ? "ENTERING" : "EXITING", idx_closest_obj);
		printf("\tincident_ray: <%.2f, %.2f, %.2f> + t<%.2f, %.2f, %.2f>\n", incident_ray.loc.x, incident_ray.loc.y, incident_ray.loc.z, incident_ray.dir.x, incident_ray.dir.y, incident_ray.dir.z);
		printf("\tsfc_isxn_point: <%.2f, %.2f, %.2f>\n", surface_intersection_point.x, surface_intersection_point.y, surface_intersection_point.z);
		printf("\tsfc_outward_normal: <%.2f, %.2f, %.2f>\n", sfc_outward_normal.x, sfc_outward_normal.y, sfc_outward_normal.z);
		printf("\tobj_type: %d\n", obj_type);
		printf("\tnum_recursions: %d\n", num_recursions);
		printf("\tni: %.2f\n", ni);
	}
	
    if (idx_closest_obj == -1) {return params.bkgcolor;}

	struct PointVec normal;

	if (enter_exit_flag == RAY_EXITING)
	{
		normal = pvmult(sfc_outward_normal, -1);
	}

	else
	{
		normal = sfc_outward_normal;
	}

	if(debug)
	{
		printf("normal (recomputed to be on near side of surface):\n");
		printf("\t<%.2f, %.2f, %.2f>\n", normal.x, normal.y, normal.z);
	}

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
		double phi = acos(sfc_outward_normal.z);
		double theta = atan2(sfc_outward_normal.y, sfc_outward_normal.x);
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

	else if(obj_type == TRIANGLE_SMOOTH_TEXTURED || obj_type == TRIANGLE_UNSMOOTH_TEXTURED)
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
			if(0)
			{
				printf("Light location:  %f, %f, %f\n", params.lights[i].locdir.x, params.lights[i].locdir.y, params.lights[i].locdir.z);
				printf("Surfc location: %f, %f, %f\n", surface_intersection_point.x, surface_intersection_point.y, surface_intersection_point.z);
				printf("Ray_to_light  :  %f, %f, %f\n", ray_to_light.dir.x, ray_to_light.dir.y, ray_to_light.dir.z);
			}
			ray_to_light.dir = pvnormalize(ray_to_light.dir);
			if(0)
			{
				printf("Ray_to_light_n:  %f, %f, %f\n", ray_to_light.dir.x, ray_to_light.dir.y, ray_to_light.dir.z);
			}
        }
        else // Directional light
        {
            ray_to_light.dir = pvnormalize(pvmult(params.lights[i].locdir, -1));
        }

        float shadow_pct = 0;

        // Determine whether the current light is shadowed
        for (int j = 0; j < params.numspheres && shadow_pct < 1; j++)
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
				if(0)
				{	
					printf("t1: %f\nt2: %f\n", (-B + sqrt(disc)) / 2, (-B - sqrt(disc)) / 2);
				}
                double t = smallest_positive((-B + sqrt(disc)) / 2, (-B - sqrt(disc)) / 2);
				if(0)
				{	
					printf("t: %f\n", t);
					printf("ray_to_light.loc: %f, %f, %f\n", ray_to_light.loc.x, ray_to_light.loc.y, ray_to_light.loc.z);
					printf("t to light: %f\n", pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))));
				}
                if ((j != idx_closest_obj || obj_type > 1) && (t > 0.0000000001 && (params.lights[i].type == 0 || t <= pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))))))
                {
                    shadow_pct += params.mtlcolors[params.spheres[j].color_idx].opacity;
                }
            }
        }

		for (int j = 0; j < params.numtriangles && shadow_pct < 1; j++)
		{
			struct PointVec e_1 = pvadd(params.vertices[params.triangles[j].v2_idx], pvmult(params.vertices[params.triangles[j].v1_idx], -1));
			struct PointVec e_2 = pvadd(params.vertices[params.triangles[j].v3_idx], pvmult(params.vertices[params.triangles[j].v1_idx], -1));
			struct PointVec n = pvnormalize(pvcross(e_1, e_2));

			double D = -(n.x * params.vertices[params.triangles[j].v1_idx].x + n.y * params.vertices[params.triangles[j].v1_idx].y + n.z * params.vertices[params.triangles[j].v1_idx].z);
			
			if(0)
			{
				printf("checking triangle %d\n", i);
				printf("n: %f, %f, %f\n", n.x, n.y, n.z);
				printf("D: %f\n", D);
			}

			if(n.x * ray_to_light.dir.x + n.y * ray_to_light.dir.y + n.z * ray_to_light.dir.z != 0)
			{
				double t = -(n.x * ray_to_light.loc.x + n.y * ray_to_light.loc.y + n.z * ray_to_light.loc.z + D)/(n.x * ray_to_light.dir.x + n.y * ray_to_light.dir.y + n.z * ray_to_light.dir.z);
				if((j != idx_closest_obj || obj_type < 2) && (t > 0.000000001 && (params.lights[i].type == 0 || t <= pvmagnitude(pvadd(params.lights[i].locdir, pvmult(surface_intersection_point, -1))))))
				{
					struct PointVec plane_intersection_point = pvadd(ray_to_light.loc, pvmult(ray_to_light.dir, t));
					struct PointVec ep = pvadd(plane_intersection_point, pvmult(params.vertices[params.triangles[j].v1_idx], -1));

					if(0)
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
							shadow_pct += params.mtlcolors[params.triangles[j].color_idx].opacity;
						}
					}
				}
			}
		}

		if(0)
		{
			printf("shadow pct onto object %d: %lf\n", idx_closest_obj, shadow_pct);
		}

        if (shadow_pct < 1) // Not fully shadowed, get diffuse & specular components
        {
            struct PointVec l = ray_to_light.dir;
            struct PointVec h = pvnormalize(pvadd(l, pvmult(incident_ray.dir, -1)));

            double nl = fmax(0.0, pvdot(sfc_outward_normal, l));
            double nhn = pow(fmax(0.0, pvdot(sfc_outward_normal, h)), mtlcolor.n);

            final_color.r += (1-shadow_pct) * (mtlcolor.od.r * mtlcolor.kd * nl + mtlcolor.os.r * mtlcolor.ks * nhn) * params.lights[i].intensity;
            final_color.g += (1-shadow_pct) * (mtlcolor.od.g * mtlcolor.kd * nl + mtlcolor.os.g * mtlcolor.ks * nhn) * params.lights[i].intensity;
            final_color.b += (1-shadow_pct) * (mtlcolor.od.b * mtlcolor.kd * nl + mtlcolor.os.b * mtlcolor.ks * nhn) * params.lights[i].intensity;
        }
    }

	if (num_recursions < MAX_RECURSION_DEPTH) {
    struct PointVec I = pvmult(incident_ray.dir, -1);

    double nt = mtlcolor.ior;

    double F0 = pow((ni - nt) / (ni + nt), 2);
    double F = fmin(1.0, F0 + (1 - F0) * pow(1 - pvdot(normal, I) / (pvmagnitude(normal) * pvmagnitude(I)), 5));

    if (debug) {
        printf("in recursive calls:\n");
        printf("\tI: <%.2f, %.2f, %.2f>\n", I.x, I.y, I.z);
        printf("\tni: %.2f\n", ni);
        printf("\tnt: %.2f\n", nt);
        printf("\tF0: %.2f\n", F0);
        printf("\tF:  %.2f\n", F);
    }

    if (mtlcolor.ks > 0) {
        struct Ray reflected_ray;
        reflected_ray.loc = surface_intersection_point;
        reflected_ray.dir = pvadd(pvmult(I, -1), pvmult(normal, 2 * pvdot(normal, I)));
        if (debug) {
            printf("\treflected ray: <%.2f, %.2f, %.2f> + t<%.2f, %.2f, %.2f>\n", reflected_ray.loc.x, reflected_ray.loc.y, reflected_ray.loc.z, reflected_ray.dir.x, reflected_ray.dir.y, reflected_ray.dir.z);
        }

        struct Color reflection_color = trace_ray(reflected_ray, num_recursions + 1, ni);

        // Total internal reflection
        if (1 - (pow((ni / nt), 2) * (1 - pow(pvdot(normal, I), 2))) < 0 && mtlcolor.opacity < 1) {
            final_color.r += reflection_color.r * (1 - F) * (1 - mtlcolor.opacity);
            final_color.g += reflection_color.g * (1 - F) * (1 - mtlcolor.opacity);
            final_color.b += reflection_color.b * (1 - F) * (1 - mtlcolor.opacity);
        }

        final_color.r += reflection_color.r * F;
        final_color.g += reflection_color.g * F;
        final_color.b += reflection_color.b * F;
    }
    
    if (mtlcolor.opacity < 1 && 1 - (pow((ni / nt), 2) * (1 - pow(pvdot(normal, I), 2))) > 0) {
        struct Ray refracted_ray;
        refracted_ray.loc = surface_intersection_point;
        refracted_ray.dir = pvmult(pvmult(normal, -1), pow(1 - (pow((ni / nt), 2) * (1 - pow(pvdot(normal, I), 2))), 0.5));
        refracted_ray.dir = pvadd(pvmult(pvadd(pvmult(normal, pvdot(normal, I)), pvmult(I, -1)), (ni / nt)), refracted_ray.dir);
        if (debug) {
            printf("\trefracted ray: <%.2f, %.2f, %.2f> + t<%.2f, %.2f, %.2f>\n", refracted_ray.loc.x, refracted_ray.loc.y, refracted_ray.loc.z, refracted_ray.dir.x, refracted_ray.dir.y, refracted_ray.dir.z);
        }
        
        struct Color refraction_color = trace_ray(refracted_ray, num_recursions + 1, (enter_exit_flag == RAY_ENTERING ? nt : params.ior_bkg));
        final_color.r += refraction_color.r * (1 - F) * (1 - mtlcolor.opacity);
        final_color.g += refraction_color.g * (1 - F) * (1 - mtlcolor.opacity);
        final_color.b += refraction_color.b * (1 - F) * (1 - mtlcolor.opacity);
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



struct Color trace_ray(struct Ray ray, int num_recursions, double cur_ior)
{
	double dist_closest_obj = __DBL_MAX__;
	int idx_closest_obj = -1;
	int obj_type = -1;
	struct PointVec surface_intersection_point;
	struct PointVec normal;

	if(0)
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
			if(t > 0.0000001 && t < dist_closest_obj)
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
		
		if(0)
		{
			printf("checking triangle %d\n", i);
			printf("n: %f, %f, %f\n", n.x, n.y, n.z);
			printf("D: %f\n", D);
		}

		if(n.x * ray.dir.x + n.y * ray.dir.y + n.z * ray.dir.z != 0)
		{
			double t = -(n.x * ray.loc.x + n.y * ray.loc.y + n.z * ray.loc.z + D)/(n.x * ray.dir.x + n.y * ray.dir.y + n.z * ray.dir.z);
			if(t > 0.0000001 && t < dist_closest_obj)
			{
				struct PointVec plane_intersection_point = pvadd(ray.loc, pvmult(ray.dir, t));
				struct PointVec ep = pvadd(plane_intersection_point, pvmult(params.vertices[params.triangles[i].v1_idx], -1));

				if(0)
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
					if((beta + gamma) <= 1 && (beta + gamma) >= 0 && (beta >= 0) && (gamma >= 0) && (beta <= 1) && (gamma <= 1))
					{
						dist_closest_obj = t;
						idx_closest_obj = i;
						obj_type = params.triangles[i].type;
						surface_intersection_point = plane_intersection_point;
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
						}
					}
				}
			}
		}
	}

	int enter_exit_flag = RAY_EXITING;

	if(pvdot(normal, ray.dir) < 0)
	{
		enter_exit_flag = RAY_ENTERING;
	}

	return shade_ray(idx_closest_obj, ray, enter_exit_flag, surface_intersection_point, normal, obj_type, num_recursions + 1, cur_ior);
}