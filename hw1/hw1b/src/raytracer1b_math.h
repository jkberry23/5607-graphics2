#ifndef RAYTRACER1B_MATH_H
#define RAYTRACER1B_MATH_H

#include <math.h>
#include "raytracer1b.h"

double pvmagnitude(struct PointVec vec);

struct PointVec pvnormalize(struct PointVec vec);

struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2);

struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2);

struct PointVec pvmult(struct PointVec vec, double n);

struct Color shade_ray(int idx_closest_obj, struct PointVec surface_intersection_point);

struct Color trace_ray(struct Ray ray);


#endif // RAYTRACER1B_MATH_H