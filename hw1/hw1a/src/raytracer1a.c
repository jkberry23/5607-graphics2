#include "raytracer1a.h"
#include "raytracer1a_io.h"
#include "raytracer1a_math.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

struct Params params;
int debug0 = 0;
int debug1 = 0;
int debug2 = 0;
int debug3 = 0;

int main(int argc, char *argv[]) {
	if(argc != 2)
	{
		printf("use: %s <input_filename>\n", argv[0]);
        return 1;
	}

	char input_filepath[80];
	char output_filepath[80];
	strcpy(input_filepath, "./input/");
	strcat(input_filepath, argv[1]);

	strcpy(output_filepath, "./output/");
	strcat(output_filepath, argv[1]);
	int l = strlen(output_filepath);

	output_filepath[l-3] = 'p';
	output_filepath[l-2] = 'p';
	output_filepath[l-1] = 'm';

	if(debug0)
	{
		printf("input_filepath: %s\n", input_filepath);
		printf("output_filepath: %s\n", output_filepath);
	}

	read_input_file(input_filepath);

	struct Color outpixels[params.imgheight][params.imgwidth];

	struct PointVec n = pvnormalize(params.viewdir);

	if(debug1)
	{
		printf("n.x: %f\n", n.x);
		printf("n.y: %f\n", n.y);
		printf("n.z: %f\n", n.z);
	}

	struct PointVec u_prime = pvcross(params.viewdir, params.updir);
	if (pvmagnitude(u_prime) == 0) 
	{
		printf("up direction co-linear to viewing direction\n");
		return 1;
	}

	if(debug1)
	{
		printf("u_prime.x: %f\n", u_prime.x);
		printf("u_prime.y: %f\n", u_prime.y);
		printf("u_prime.z: %f\n", u_prime.z);
	}

	struct PointVec u = pvnormalize(u_prime);

	if(debug1)
	{
		printf("u.x: %f\n", u.x);
		printf("u.y: %f\n", u.y);
		printf("u.z: %f\n", u.z);
	}

	struct PointVec v = pvcross(u, n);

	if(debug1)
	{
		printf("v.x: %f\n", v.x);
		printf("v.y: %f\n", v.y);
		printf("v.z: %f\n", v.z);
	}

	double d = 5;
	double w = 2 * d * tan((params.fovh * PI / 180) / 2);
	double h = w / ((double)params.imgwidth/params.imgheight);

	if(debug1)
	{
		printf("w: %f\n", w);
		printf("h: %f\n", h);
	}

	struct PointVec ul = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, -w/2), pvmult(v, h/2))));
	struct PointVec ur = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, w/2), pvmult(v, h/2))));
	struct PointVec ll = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, -w/2), pvmult(v, -h/2))));
	struct PointVec lr = pvadd(params.eye, pvadd(pvmult(n, d), pvadd(pvmult(u, w/2), pvmult(v, -h/2))));

	if(debug2)
	{
		printf("ul: <%f, %f, %f>\n", ul.x, ul.y, ul.z);
		printf("ur: <%f, %f, %f>\n", ur.x, ur.y, ur.z);
		printf("ll: <%f, %f, %f>\n", ll.x, ll.y, ll.z);
		printf("lr: <%f, %f, %f>\n", lr.x, lr.y, lr.z);
	}

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

	if(debug2)
	{
		printf("dv: <%f, %f, %f>\n", dv.x, dv.y, dv.z);
		printf("dh: <%f, %f, %f>\n", dh.x, dh.y, dh.z);
	}

	for(int i = 0; i < params.imgheight; i++)
	{
		struct PointVec curpx_row = pvadd(ul, pvmult(dv, i));

		for(int j = 0; j < params.imgwidth; j++)
		{
			struct PointVec curpx = pvadd(curpx_row, pvmult(dh, j));
			struct Ray to_trace;
			to_trace.loc = params.eye;
			to_trace.dir = pvnormalize(pvadd(curpx, pvmult(params.eye, -1)));

			if(debug2)
			{
				printf("<col, row: <%d, %d>\n", j, i);
				printf("ray passing through: <%f, %f, %f>\n", curpx.x, curpx.y, curpx.z);
				printf("ray equation: <%f, %f, %f> + t*<%f, %f, %f>\n", to_trace.loc.x, to_trace.loc.y, to_trace.loc.z, to_trace.dir.x, to_trace.dir.y, to_trace.dir.z);
			}

			outpixels[i][j] = trace_ray(to_trace);
		}
	}

	write_output_file(output_filepath, outpixels);

	free(params.mtlcolors);
	free(params.spheres);

	return 0;
}