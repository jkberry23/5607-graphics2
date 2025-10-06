#ifndef RAYTRACER1D_H
#define RAYTRACER1D_H

#include <stdlib.h>

#define COLOR_BOUND 255
#define PI 3.14159265358979323846264338327950288
#define SPHERE_TEXTURED 0
#define SPHERE_UNTEXTURED 1
#define TRIANGLE_SMOOTH_TEXTURED 2
#define TRIANGLE_UNSMOOTH_TEXTURED 3
#define TRIANGLE_SMOOTH_UNTEXTURED 4
#define TRIANGLE_UNSMOOTH_UNTEXTURED 5
#define MAX_RECURSION_DEPTH 10

#define RAY_ENTERING 0
#define RAY_EXITING 1

struct PointVec {
    double x;
    double y;
    double z;
};

struct Texture {
    int height;
    int width;
    struct Color **pixels;
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

    double opacity;
    double ior;
};

struct Sphere {
    struct PointVec c;
    double r;
    int color_idx;
    int type;
    int texture_idx;
};

struct VText {
    double u;
    double v;
};

struct Triangle {
    int v1_idx;
    int v2_idx;
    int v3_idx;
    int color_idx;
    int texture_idx;
    int type;
    int v1_norm_idx;
    int v2_norm_idx;
    int v3_norm_idx;
    int v1_text_idx;
    int v2_text_idx;
    int v3_text_idx;
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
    double fovh;
    struct PointVec updir;
    struct Color bkgcolor;
    double ior_bkg;

    int nummtlcolors;
    int numspheres;
    int numtriangles;
    int numlights;
    int numvertices;
    int numvnorms;
    int numvtexts;
    int numtextures;
    int cur_ior_stacksize;

    double ior_stack[100];
    struct MtlColor *mtlcolors;
    struct Sphere *spheres;
    struct Triangle *triangles;
    struct Light *lights;
    struct PointVec *vertices;
    struct PointVec *vnorms;
    struct VText *vtexts;
    struct Texture *textures;
};

extern struct Params params;

double pvmagnitude(struct PointVec vec);
struct PointVec pvnormalize(struct PointVec vec);
struct PointVec pvcross(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvadd(struct PointVec vec1, struct PointVec vec2);
struct PointVec pvmult(struct PointVec vec, double n);
double pvdot(struct PointVec vec1, struct PointVec vec2);
double smallest_positive(double d1, double d2);
struct Color shade_ray(int idx_closest_obj, struct Ray incident_ray, int enter_exit_flag,
					   struct PointVec surface_intersection_point, struct PointVec sfc_outward_normal, 
					   int obj_type, int num_recursions, double ni);
struct Color trace_ray(struct Ray ray, int num_recursions, double cur_ior);

#endif // RAYTRACER1D_H
