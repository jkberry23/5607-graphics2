#ifndef RAYTRACER1B_H
#define RAYTRACER1B_H

#include <stdlib.h>

#define COLOR_BOUND 255
#define PI 3.14159265358979323846264338327950288

struct PointVec {
    double x;
    double y;
    double z;
};

struct Color {
    double r;
    double g;
    double b;
};

struct MtlColor {
    struct Color od;
    struct Color os;

    double ka;
    double kd;
    double ks;

    int n;
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

struct Light {
    struct PointVec locdir;
    int type;
    double intensity;
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
    int numlights;

    struct MtlColor *mtlcolors;
    struct Sphere *spheres;
    struct Light *lights;
};

extern struct Params params;
extern int debug0;
extern int debug1;
extern int debug2;
extern int debug3;
extern int debug4;



double pvmagnitude(struct PointVec vec);
struct PointVec pvnormalize(struct PointVec vec);
struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvmult(struct PointVec vec, double n);
struct Color shade_ray(int idx_closest_obj, struct PointVec surface_intersection_point);
struct Color trace_ray(struct Ray ray);

#endif // RAYTRACER1B_H
