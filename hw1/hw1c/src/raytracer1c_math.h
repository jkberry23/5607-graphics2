#ifndef RAYTRACER1C_MATH_H
#define RAYTRACER1C_MATH_H


#include "utils.h"
#include "raytracer1c.h"

struct Color shade_ray(int idx_closest_obj, struct PointVec surface_intersection_point, struct PointVec normal, int obj_type);

struct Color trace_ray(struct Ray ray);


#endif // RAYTRACER1C_MATH_H