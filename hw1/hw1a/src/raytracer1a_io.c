#include "raytracer1a_io.h"
#include "raytracer1a.h"

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
			params.bkgcolor.r = (atof(arg1) * COLOR_BOUND);
			params.bkgcolor.g = (atof(arg2) * COLOR_BOUND);
			params.bkgcolor.b = (atof(arg3) * COLOR_BOUND);

		}

		else if(strcmp(keyword, "mtlcolor") == 0)
		{
			curline++;
			params.nummtlcolors++;
			if(fscanf(infile, "%s %s %s", arg1, arg2, arg3) < 3)
			{
				printf("error parsing line %d (material color)\n", curline);
				exit(1);
			}
			if(atof(arg1) > 1 || atof(arg1) < 0 || atof(arg2) > 1 || atof(arg2) < 0 || atof(arg3) > 1 || atof(arg3) < 0)
			{
				printf("line %d: mtlcolor rgb values must be in the range [0,1]\n", curline);
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

	params.mtlcolors = (struct Color*)malloc(params.nummtlcolors*sizeof(struct Color));
	memset(params.mtlcolors, 0, params.nummtlcolors*sizeof(struct Color));
	params.spheres = (struct Sphere*)malloc(params.numspheres*sizeof(struct Sphere));
	memset(params.spheres, 0, params.numspheres*sizeof(struct Sphere));

	fseek(infile, 0, SEEK_SET);
	int idx_cur_color = -1;
	int idx_cur_sphere = 0;

	while(fscanf(infile, "%s", keyword) != EOF) 
	{
		if(strcmp(keyword, "mtlcolor") == 0)
		{
			idx_cur_color++;
			fscanf(infile, "%s %s %s", arg1, arg2, arg3);
			params.mtlcolors[idx_cur_color].r = (atof(arg1) * COLOR_BOUND); 
			params.mtlcolors[idx_cur_color].g = (atof(arg2) * COLOR_BOUND);
			params.mtlcolors[idx_cur_color].b = (atof(arg3) * COLOR_BOUND);
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
	
		printf("bkgcolor.r: %d\n", params.bkgcolor.r);
		printf("bkgcolor.g: %d\n", params.bkgcolor.g);
		printf("bkgcolor.b: %d\n", params.bkgcolor.b);
	
		printf("params.nummtlcolors: %d\n", params.nummtlcolors);
		printf("params.numspheres: %d\n", params.numspheres);

		for(int i = 0; i < params.nummtlcolors; i++)
		{
			printf("params.mtlcolors[%d]:\n", i);
			printf("\tr: %d\n", params.mtlcolors[i].r);
			printf("\tg: %d\n", params.mtlcolors[i].g);
			printf("\tb: %d\n", params.mtlcolors[i].b);
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
			sprintf(numbuf, "%d", outpixels[i][j].r);
			fwrite(numbuf, strlen(numbuf), 1, outfile);
			fwrite(" ", strlen(" "), 1, outfile);
			sprintf(numbuf, "%d", outpixels[i][j].g);
			fwrite(numbuf, strlen(numbuf), 1, outfile);
			fwrite(" ", strlen(" "), 1, outfile);
			sprintf(numbuf, "%d", outpixels[i][j].b);
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