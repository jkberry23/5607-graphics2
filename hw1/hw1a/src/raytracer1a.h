#ifndef RAYTRACER1A_H
#define RAYTRACER1A_H

#include <stdlib.h>

#define COLOR_BOUND 255
#define PI 3.14159265358979323846264338327950288

struct Color {
    int r;
    int g;
    int b;
};

struct PointVec {
    double x;
    double y;
    double z;
};

struct Sphere {
    struct PointVec c;
    double r;
    int color_idx;
};

struct Ray {
    struct PointVec loc;
    struct PointVec dir;
};

struct Params {
    int imgwidth;
    int imgheight;

    struct PointVec eye;
    struct PointVec viewdir;
    int fovh;
    struct PointVec updir;
    struct Color bkgcolor;

    int nummtlcolors;
    int numspheres;

    struct Color *mtlcolors;
    struct Sphere *spheres;
};

extern struct Params params;
extern int debug0;
extern int debug1;
extern int debug2;
extern int debug3;


double pvmagnitude(struct PointVec vec);
struct PointVec pvnormalize(struct PointVec vec);
struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvmult(struct PointVec vec, double n);
struct Color shade_ray(int idx_closest_obj);
struct Color trace_ray(struct Ray ray);

#endif // RAYTRACER1A_H
