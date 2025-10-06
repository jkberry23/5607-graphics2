#include "raytracer1d.h"
#include "utils.h"
#include "raytracer1d_io.h"
#include "raytracer1d_math.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

struct Params params;

int main(int argc, char *argv[]) {
	params.cur_ior_stacksize = 0;
	if(argc != 2)
	{
		printf("use: %s <input_filename>\n", argv[0]);
        return 1;
	}

	char input_filepath[80];
	strcpy(input_filepath, "./input/");
	strcat(input_filepath, argv[1]);

	char output_filepath[80];
	strcpy(output_filepath, "./output/");
	strcat(output_filepath, argv[1]);
	int l = strlen(output_filepath);

	output_filepath[l-3] = 'p';
	output_filepath[l-2] = 'p';
	output_filepath[l-1] = 'm';

	if(0)
	{
		printf("input_filepath: %s\n", input_filepath);
		printf("output_filepath: %s\n", output_filepath);
	}

	read_input_file(input_filepath);

	struct Color outpixels[params.imgheight][params.imgwidth];

	struct PointVec n = pvnormalize(params.viewdir);

	struct PointVec u_prime = pvcross(params.viewdir, params.updir);
	if (pvmagnitude(u_prime) == 0) 
	{
		printf("up direction co-linear to viewing direction\n");
		return 1;
	}

	struct PointVec u = pvnormalize(u_prime);
	struct PointVec v = pvcross(u, n);

	double d = 5;
	double w = 2 * d * tan((params.fovh * PI / 180) / 2);
	double h = w / ((double)params.imgwidth/params.imgheight);

	if(0)
	{
		printf("n.x: %f\n", n.x);
		printf("n.y: %f\n", n.y);
		printf("n.z: %f\n", n.z);
		printf("u.x: %f\n", u.x);
		printf("u.y: %f\n", u.y);
		printf("u.z: %f\n", u.z);
		printf("v.x: %f\n", v.x);
		printf("v.y: %f\n", v.y);
		printf("v.z: %f\n", v.z);
		printf("u_prime.x: %f\n", u_prime.x);
		printf("u_prime.y: %f\n", u_prime.y);
		printf("u_prime.z: %f\n", u_prime.z);
		printf("w: %f\n", w);
		printf("h: %f\n", h);
	}

	struct PointVec ul = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, -w/2), pvmult(v, h/2))));
	struct PointVec ur = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, w/2), pvmult(v, h/2))));
	struct PointVec ll = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, -w/2), pvmult(v, -h/2))));
	struct PointVec lr = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, w/2), pvmult(v, -h/2))));

	struct PointVec dv = {0,0,0};
	if(params.imgheight > 1)
	{
		dv = pvmult(pvadd(ll, pvmult(ul, -1)), 1./(params.imgheight-1));
	}
	
	struct PointVec dh = {0,0,0};
	if(params.imgwidth > 1)
	{
		dh = pvmult(pvadd(ur, pvmult(ul, -1)), 1./(params.imgwidth-1));
	}

	if(0)
	{
		printf("ul: <%f, %f, %f>\n", ul.x, ul.y, ul.z);
		printf("ur: <%f, %f, %f>\n", ur.x, ur.y, ur.z);
		printf("ll: <%f, %f, %f>\n", ll.x, ll.y, ll.z);
		printf("lr: <%f, %f, %f>\n", lr.x, lr.y, lr.z);
		printf("dv: <%f, %f, %f>\n", dv.x, dv.y, dv.z);
		printf("dh: <%f, %f, %f>\n", dh.x, dh.y, dh.z);
	}

	for(int i = 0; i < params.imgheight; i++)
	// for(int i = 255; i < 256; i++)
	{
		struct PointVec curpx_row = pvadd(ul, pvmult(dv, i));

		for(int j = 0; j < params.imgwidth; j++)
		// for(int j = 245; j < 246; j++)
		{
			struct PointVec curpx = pvadd(curpx_row, pvmult(dh, j));
			struct Ray ray_to_trace;
			ray_to_trace.loc = params.eye;
			ray_to_trace.dir = pvnormalize(pvadd(curpx, pvmult(params.eye, -1)));

			if(0)
			{
				printf("<col, row: <%d, %d>\n", j, i);
				printf("ray passing through: <%f, %f, %f>\n", curpx.x, curpx.y, curpx.z);
				printf("ray equation: <%f, %f, %f> + t*<%f, %f, %f>\n", ray_to_trace.loc.x, ray_to_trace.loc.y, ray_to_trace.loc.z, ray_to_trace.dir.x, ray_to_trace.dir.y, ray_to_trace.dir.z);
			}

			outpixels[i][j] = trace_ray(ray_to_trace, 0, params.ior_bkg);
		}
	}

	write_output_file(output_filepath, outpixels);

	return 0;
}