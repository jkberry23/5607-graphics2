#ifndef RAYTRACER1D_MATH_H
#define RAYTRACER1D_MATH_H


#include "utils.h"
#include "raytracer1d.h"

struct Color shade_ray(int idx_closest_obj, struct Ray incident_ray, int enter_exit_flag, 
					   struct PointVec surface_intersection_point, struct PointVec sfc_outward_normal, 
					   int obj_type, int num_recursions, double ni);

struct Color trace_ray(struct Ray ray, int num_recursions, double cur_ior);


#endif // RAYTRACER1D_MATH_H