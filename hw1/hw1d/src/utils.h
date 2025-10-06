#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include "raytracer1d.h"

double pvmagnitude(struct PointVec vec);

struct PointVec pvnormalize(struct PointVec vec);

struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2);

struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2);

struct PointVec pvmult(struct PointVec vec, double n);

double pvdot(struct PointVec vec1, struct PointVec vec2);

double smallest_positive(double d1, double d2);

#endif // UTILS_H