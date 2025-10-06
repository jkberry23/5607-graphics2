#include "raytracer1b_io.h"
#include "raytracer1b.h"

#include <dirent.h>

void read_input_file(const char *input_filepath)
{
	FILE *infile = fopen(input_filepath, "r");
	
	if(infile == NULL)
	{
		printf("file does not exist (is it in the ./input/ directory?)\n");
		exit(1);
	}

	char keyword[10];
	char arg1[8];
	char arg2[8];
	char arg3[8];
	char arg4[8];
	char arg5[8];
	char arg6[8];
	char arg7[8];
	char arg8[8];
	char arg9[8];
	char arg10[8];

	int curline = 0;

	int got_imsize = 0;
	int got_eye = 0;
	int got_viewdir = 0;
	int got_hfov = 0;
	int got_updir = 0;
	int got_bkgcolor = 0;

	while(fscanf(infile, "%s", keyword) != EOF) 
	{
		curline++;
		if(strcmp(keyword, "imsize") == 0) 
		{
			got_imsize++;
			if(fscanf(infile, "%s %s", arg1, arg2) < 2)
			{
				printf("error parsing line %d (image size)\n", curline);
				exit(1);
			}
			params.imgwidth = atoi(arg1);
			params.imgheight = atoi(arg2);

			if(params.imgwidth <= 0 || params.imgheight <= 0)
			{
				printf("line %d: imgsize parameters invalid; both must be > 0\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "eye") == 0)
		{
			got_eye++;
			if(fscanf(infile, "%s %s %s", arg1, arg2, arg3) < 3)
			{
				printf("error parsing line %d (eye position)\n", curline);
				exit(1);
			}
			params.eye.x = atof(arg1);
			params.eye.y = atof(arg2);
			params.eye.z = atof(arg3);
		}

		else if(strcmp(keyword, "viewdir") == 0)
		{
			got_viewdir++;
			if(fscanf(infile, "%s %s %s", arg1, arg2, arg3) < 3)
			{
				printf("error parsing line %d (viewing direction)\n", curline);
				exit(1);
			}
			params.viewdir.x = atof(arg1);
			params.viewdir.y = atof(arg2);
			params.viewdir.z = atof(arg3);
		}

		else if(strcmp(keyword, "hfov") == 0)
		{
			got_hfov++;
			if(fscanf(infile, "%s", arg1) < 1)
			{
				printf("error parsing line %d (horizontal fov)\n", curline);
				exit(1);
			}
			params.fovh = atoi(arg1);

			if(params.fovh <= 0 || params.fovh >= 180)
			{
				printf("line %d: hfov parameter invalid; must be in the range (0,180)\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "updir") == 0)
		{
			got_updir++;
			if(fscanf(infile, "%s %s %s", arg1, arg2, arg3) < 3)
			{
				printf("error parsing line %d (up direction)\n", curline);
				exit(1);
			}
			params.updir.x = atof(arg1);
			params.updir.y = atof(arg2);
			params.updir.z = atof(arg3);
		}

		else if(strcmp(keyword, "bkgcolor") == 0)
		{
			got_bkgcolor++;
			if(fscanf(infile, "%s %s %s", arg1, arg2, arg3) < 3)
			{
				printf("error parsing line %d (background color)\n", curline);
				exit(1);
			}

			if(atof(arg1) > 1 || atof(arg1) < 0 || atof(arg2) > 1 || atof(arg2) < 0 || atof(arg3) > 1 || atof(arg3) < 0)
			{
				printf("line %d: bkgcolor rgb values must be in the range [0,1]\n", curline);
				exit(1);
			}
			params.bkgcolor.r = atof(arg1);
			params.bkgcolor.g = atof(arg2);
			params.bkgcolor.b = atof(arg3);

		}

		else if(strcmp(keyword, "mtlcolor") == 0)
		{
			params.nummtlcolors++;
			if(fscanf(infile, "%s %s %s %s %s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) < 3)
			{
				printf("error parsing line %d (material color)\n", curline);
				exit(1);
			}
			if(atof(arg1) > 1 || atof(arg1) < 0 || atof(arg2) > 1 || atof(arg2) < 0 || atof(arg3) > 1 || atof(arg3) < 0 ||
			   atof(arg4) > 1 || atof(arg4) < 0 || atof(arg5) > 1 || atof(arg5) < 0 || atof(arg6) > 1 || atof(arg6) < 0 ||
			   atof(arg7) > 1 || atof(arg7) < 0 || atof(arg8) > 1 || atof(arg8) < 0 || atof(arg9) > 1 || atof(arg9) < 0)
			{
				printf("line %d: mtlcolor values must be in the range [0,1]\n", curline);
				exit(1);
			}
			if(atoi(arg10) < 0)
			{
				printf("line %d: mtlcolor n value must be >= 0\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "sphere") == 0)
		{
			params.numspheres++;
			if(fscanf(infile, "%s %s %s %s", arg1, arg2, arg3, arg4) < 4)
			{
				printf("error parsing line %d (sphere)\n", curline);
				exit(1);
			}
			if(atof(arg4) < 0)
			{
				printf("line %d: sphere radius invalid; must be >= 0\n", curline);
				exit(1);
			}
		}

		else if (strcmp(keyword, "light") == 0)
		{
			params.numlights++;
			if(fscanf(infile, "%s %s %s %s %s", arg1, arg2, arg3, arg4, arg5) < 5)
			{
				printf("error parsing line %d (light)\n", curline);
				exit(1);
			}
			if(atoi(arg4) != 0 && atoi(arg4) != 1)
			{
				printf("line %d: light type must be either 0 or 1, currently %d\n", curline, atoi(arg4));
				exit(1);
			}
			if(atof(arg5) < 0 || atof(arg5) > 1)
			{
				printf("line %d: light intensity must be in range [0,1]\n", curline);
				exit(1);
			}
		}

		else
		{
			printf("invalid keyword \"%s\" encountered in or before line %d\n", keyword, curline);
		}
	}

	if(got_imsize != 1 || got_eye != 1 || got_viewdir != 1 || got_hfov != 1 || got_updir != 1 || got_bkgcolor != 1)
	{
		printf("encountered the following errors in input file:");
		if(got_imsize > 1)
		{
			printf("\timsize declared more than once");
		}
		if(got_eye > 1)
		{
			printf("\teye declared more than once");
		}
		if(got_viewdir > 1)
		{
			printf("\tviewdir declared more than once");
		}
		if(got_hfov > 1)
		{
			printf("\thfov declared more than once");
		}
		if(got_updir > 1)
		{
			printf("\tupdir declared more than once");
		}
		if(got_bkgcolor > 1)
		{
			printf("\tbkgcolor declared more than once");
		}
		if(got_imsize < 1)
		{
			printf("\timsize not declared");
		}
		if(got_eye < 1)
		{
			printf("\teye not declared");
		}
		if(got_viewdir < 1)
		{
			printf("\tviewdir not declared");
		}
		if(got_hfov < 1)
		{
			printf("\thfov not declared");
		}
		if(got_updir < 1)
		{
			printf("\tupdir not declared");
		}
		if(got_bkgcolor < 1)
		{
			printf("\tbkgcolor not declared");
		}
		exit(1);
	}

	if(params.numspheres > 86) {
		printf("exceeded max # spheres");
		exit(1);
	}

	params.mtlcolors = (struct MtlColor*)malloc(params.nummtlcolors*sizeof(struct MtlColor));
	memset(params.mtlcolors, 0, params.nummtlcolors*sizeof(struct MtlColor));
	params.spheres = (struct Sphere*)malloc(params.numspheres*sizeof(struct Sphere));
	memset(params.spheres, 0, params.numspheres*sizeof(struct Sphere));
	params.lights = (struct Light*)malloc(params.numlights*sizeof(struct Light));
	memset(params.lights, 0, params.numlights*sizeof(struct Light));

	fseek(infile, 0, SEEK_SET);
	int idx_cur_color = -1;
	int idx_cur_sphere = 0;
	int idx_cur_light = 0;

	while(fscanf(infile, "%s", keyword) != EOF) 
	{
		if(strcmp(keyword, "mtlcolor") == 0)
		{
			idx_cur_color++;
			fscanf(infile, "%s %s %s %s %s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
			params.mtlcolors[idx_cur_color].od.r = atof(arg1); 
			params.mtlcolors[idx_cur_color].od.g = atof(arg2);
			params.mtlcolors[idx_cur_color].od.b = atof(arg3);

			params.mtlcolors[idx_cur_color].os.r = atof(arg4); 
			params.mtlcolors[idx_cur_color].os.g = atof(arg5);
			params.mtlcolors[idx_cur_color].os.b = atof(arg6);

			params.mtlcolors[idx_cur_color].ka = atof(arg7);
			params.mtlcolors[idx_cur_color].kd = atof(arg8);
			params.mtlcolors[idx_cur_color].ks = atof(arg9);

			params.mtlcolors[idx_cur_color].n = atoi(arg10);
		}

		else if(strcmp(keyword, "sphere") == 0)
		{
			if(idx_cur_color < 0) {
				printf("keyword \"sphere\" encountered before any material color defined");
				exit(1);
			}
			fscanf(infile, "%s %s %s %s", arg1, arg2, arg3, arg4); 
			params.spheres[idx_cur_sphere].c.x = atof(arg1);
			params.spheres[idx_cur_sphere].c.y = atof(arg2);
			params.spheres[idx_cur_sphere].c.z = atof(arg3);
			params.spheres[idx_cur_sphere].r = atof(arg4);
			params.spheres[idx_cur_sphere].color_idx = idx_cur_color;
			idx_cur_sphere++;
		}

		else if(strcmp(keyword, "light") == 0)
		{
			fscanf(infile, "%s %s %s %s %s", arg1, arg2, arg3, arg4, arg5); 
			params.lights[idx_cur_light].locdir.x = atof(arg1);
			params.lights[idx_cur_light].locdir.y = atof(arg2);
			params.lights[idx_cur_light].locdir.z = atof(arg3);
			params.lights[idx_cur_light].type = atoi(arg4);
			params.lights[idx_cur_light].intensity = atof(arg5);
			idx_cur_light++;
			
		}
	}

	if(debug0)
	{
		printf("imgwidth: %d\n", params.imgwidth);
		printf("imgheight: %d\n", params.imgheight);

		printf("eye_x: %f\n", params.eye.x);
		printf("eye_y: %f\n", params.eye.y);
		printf("eye_z: %f\n", params.eye.z);

		printf("viewdir_x: %f\n", params.viewdir.x);
		printf("viewdir_y: %f\n", params.viewdir.y);
		printf("viewdir_z: %f\n", params.viewdir.z);
	
		printf("fovh: %d\n", params.fovh);

		printf("updir_x: %f\n", params.updir.x);
		printf("updir_y: %f\n", params.updir.y);
		printf("updir_z: %f\n", params.updir.z);
	
		printf("bkgcolor.r: %f\n", params.bkgcolor.r);
		printf("bkgcolor.g: %f\n", params.bkgcolor.g);
		printf("bkgcolor.b: %f\n", params.bkgcolor.b);
	
		printf("params.nummtlcolors: %d\n", params.nummtlcolors);
		printf("params.numspheres: %d\n", params.numspheres);

		for(int i = 0; i < params.nummtlcolors; i++)
		{
			printf("params.mtlcolors[%d]:\n", i);
			printf("\tosr: %f\n", params.mtlcolors[i].os.r);
			printf("\tosg: %f\n", params.mtlcolors[i].os.g);
			printf("\tosb: %f\n", params.mtlcolors[i].os.b);
			printf("\todr: %f\n", params.mtlcolors[i].od.r);
			printf("\todg: %f\n", params.mtlcolors[i].od.g);
			printf("\todb: %f\n", params.mtlcolors[i].od.b);
			printf("\tka: %f\n", params.mtlcolors[i].ka);
			printf("\tkd: %f\n", params.mtlcolors[i].kd);
			printf("\tks: %f\n", params.mtlcolors[i].ks);
			printf("\tn: %d\n", params.mtlcolors[i].n);
		}

		for(int i = 0; i < params.numspheres; i++)
		{
			printf("params.spheres[%d]:\n", i);
			printf("\tx_c: %f\n", params.spheres[i].c.x);
			printf("\ty_c: %f\n", params.spheres[i].c.x);
			printf("\tz_c: %f\n", params.spheres[i].c.z);
			printf("\tr: %f\n", params.spheres[i].r);
			printf("\tcolor_idx: %d\n", params.spheres[i].color_idx);
		}
	}
	fclose(infile);
}

void write_output_file(const char *output_filepath, struct Color outpixels[params.imgheight][params.imgwidth])
{
	FILE *outfile = fopen(output_filepath, "w");

	char preamble[40];
	strcpy(preamble, "P3\n");
	char numbuf[10];
	sprintf(numbuf, "%d", params.imgwidth);
	strcat(preamble, numbuf);
	strcat(preamble, " ");
	sprintf(numbuf, "%d", params.imgheight);
	strcat(preamble, numbuf);
	strcat(preamble, "\n255\n");
	fwrite(preamble, strlen(preamble), 1, outfile);

	for(int i = 0; i < params.imgheight; i++)
	{
		for(int j = 0; j < params.imgwidth; j++)
		{
			sprintf(numbuf, "%d", (int)(outpixels[i][j].r * COLOR_BOUND));
			fwrite(numbuf, strlen(numbuf), 1, outfile);
			fwrite(" ", strlen(" "), 1, outfile);
			sprintf(numbuf, "%d", (int)(outpixels[i][j].g * COLOR_BOUND));
			fwrite(numbuf, strlen(numbuf), 1, outfile);
			fwrite(" ", strlen(" "), 1, outfile);
			sprintf(numbuf, "%d", (int)(outpixels[i][j].b * COLOR_BOUND));
			fwrite(numbuf, strlen(numbuf), 1, outfile);
			if(j % 5 == 0 && j != 0)
			{
				fwrite("\n", strlen("\n"), 1, outfile);
			}
			else
			{
				fwrite(" ", strlen(" "), 1, outfile);
			}
		}
	}

	fflush(outfile);
	fclose(outfile);
}