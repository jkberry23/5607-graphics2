#include "raytracer1c_io.h"
#include "raytracer1c.h"

#include <dirent.h>

void read_input_file(const char *input_filepath)
{
	FILE *infile = fopen(input_filepath, "r");
	
	if(infile == NULL)
	{
		printf("file does not exist (is it in the ./input/ directory?)\n");
		exit(1);
	}

	char line[100];
	char keyword[10];
	char arg1[20];
	char arg2[20];
	char arg3[20];
	char arg4[20];
	char arg5[20];
	char arg6[20];
	char arg7[20];
	char arg8[20];
	char arg9[20];
	char arg10[20];

	double f1;
	double f2;
	double f3;
	double f4;
	double f5;
	double f6;
	double f7;
	double f8;
	double f9;
	double f10;
	
	int i1;
	int i2;
	int i3;
	int i4;
	int i5;
	int i6;
	int i7;
	int i8;
	int i9;
	int i10;

	int curline = 0;

	int got_imsize = 0;
	int got_eye = 0;
	int got_viewdir = 0;
	int got_hfov = 0;
	int got_updir = 0;
	int got_bkgcolor = 0;
	int got_texture = 0;

	while(fscanf(infile, "%s", keyword) != EOF) 
	{
		curline++;
		if(strcmp(keyword, "imsize") == 0) 
		{
			got_imsize++;
			fgets(line, 100, infile);
			if(sscanf(line, "%d %d", &i1, &i2) < 2)
			{
				printf("error parsing line %d (image size)\n", curline);
				exit(1);
			}
			params.imgwidth = i1;
			params.imgheight = i2;

			if(params.imgwidth <= 0 || params.imgheight <= 0)
			{
				printf("line %d: imgsize parameters invalid; both must be > 0\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "eye") == 0)
		{
			got_eye++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (eye position)\n", curline);
				exit(1);
			}
			params.eye.x = f1;
			params.eye.y = f2;
			params.eye.z = f3;
		}

		else if(strcmp(keyword, "viewdir") == 0)
		{
			got_viewdir++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (viewing direction)\n", curline);
				exit(1);
			}
			params.viewdir.x = f1;
			params.viewdir.y = f2;
			params.viewdir.z = f3;
		}

		else if(strcmp(keyword, "hfov") == 0)
		{
			got_hfov++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf", &f1) < 1)
			{
				printf("error parsing line %d (horizontal fov)\n", curline);
				exit(1);
			}
			params.fovh = f1;

			if(params.fovh <= 0 || params.fovh >= 180)
			{
				printf("line %d: hfov parameter invalid; must be in the range (0,180)\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "updir") == 0)
		{
			got_updir++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (up direction)\n", curline);
				exit(1);
			}
			params.updir.x = f1;
			params.updir.y = f2;
			params.updir.z = f3;
		}

		else if(strcmp(keyword, "bkgcolor") == 0)
		{
			got_bkgcolor++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (background color)\n", curline);
				exit(1);
			}

			if(f1 > 1 || f1 < 0 || f2 > 1 || f2 < 0 || f3 > 1 || f3 < 0)
			{
				printf("line %d: bkgcolor rgb values must be in the range [0,1]\n", curline);
				exit(1);
			}
			params.bkgcolor.r = f1;
			params.bkgcolor.g = f2;
			params.bkgcolor.b = f3;
		}

		else if(strcmp(keyword, "mtlcolor") == 0)
		{
			params.nummtlcolors++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %d", &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &i1) < 3)
			{
				printf("error parsing line %d (material color)\n", curline);
				exit(1);
			}
			if(f1 > 1 || f1 < 0 || f2 > 1 || f2 < 0 || f3 > 1 || f3 < 0 ||
			   f4 > 1 || f4 < 0 || f5 > 1 || f5 < 0 || f6 > 1 || f6 < 0 ||
			   f7 > 1 || f7 < 0 || f8 > 1 || f8 < 0 || f9 > 1 || f9 < 0)
			{
				printf("line %d: mtlcolor values must be in the range [0,1]\n", curline);
				exit(1);
			}
			if(i1 < 0)
			{
				printf("line %d: mtlcolor n value must be >= 0\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "sphere") == 0)
		{
			params.numspheres++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf %lf", &f1, &f2, &f3, &f4) < 4)
			{
				printf("error parsing line %d (sphere)\n", curline);
				exit(1);
			}
			if(f4 < 0)
			{
				printf("line %d: sphere radius invalid; must be >= 0\n", curline);
				exit(1);
			}
			if(params.nummtlcolors == 0)
			{
				printf("keyword \"sphere\" encountered before any material color defined");
				exit(1);
			}
		}

		else if(strcmp(keyword, "v") == 0)
		{
			params.numvertices++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (vertex)\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "vn") == 0)
		{
			params.numvnorms++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf", &f1, &f2, &f3) < 3)
			{
				printf("error parsing line %d (vertex normal)\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "vt") == 0)
		{
			params.numvtexts++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf", &f1, &f2) < 2)
			{
				printf("%s\n", line);
				printf("error parsing line %d (vertex texture)\n", curline);
				exit(1);
			}
		}

		else if(strcmp(keyword, "f") == 0)
		{
			params.numtriangles++;
			fgets(line, 100, infile);
			if(params.nummtlcolors == 0)
			{
				printf("keyword \"f\" encountered before any material color defined");
				exit(1);
			}
			if (sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d", &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9) == 9)
			{
				if(params.numtextures == 0)
				{
					printf("texture coordinates given before a texture was specified");
					exit(1);
				}
				if(i1 < 1 || i1 > params.numvertices || i4 < 1 || i4 > params.numvertices || i7 < 1 || i7 > params.numvertices)
				{
					printf("vertex referenced in triangle before its instantiation");
					exit(1);
				}
				if(i2 < 1 || i2 > params.numvtexts || i5 < 1 || i5 > params.numvtexts || i8 < 1 || i8 > params.numvtexts)
				{
					printf("vertex normal referenced in triangle before its instantiation");
					exit(1);
				}
				if(i3 < 1 || i3 > params.numvnorms || i6 < 1 || i6 > params.numvnorms || i9 < 1 || i9 > params.numvnorms)
				{
					printf("vertex normal referenced in triangle before its instantiation");
					exit(1);
				}
			}
			else if (sscanf(line, "%d/%d %d/%d %d/%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6)
			{
				if(params.numtextures == 0)
				{
					printf("texture coordinates given before a texture was specified");
					exit(1);
				}
				if(i1 < 1 || i1 > params.numvertices || i3 < 1 || i3 > params.numvertices || i5 < 1 || i5 > params.numvertices)
				{
					printf("vertex referenced in triangle before its instantiation");
					exit(1);
				}
				if(i2 < 1 || i2 > params.numvtexts || i4 < 1 || i4 > params.numvtexts || i6 < 1 || i6 > params.numvtexts)
				{
					printf("vertex texture referenced in triangle before its instantiation");
					exit(1);
				}
			}
			else if (sscanf(line, "%d//%d %d//%d %d//%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6) 
			{
				if(i1 < 1 || i1 > params.numvertices || i3 < 1 || i3 > params.numvertices || i5 < 1 || i5 > params.numvertices)
				{
					printf("vertex referenced in triangle before its instantiation");
					exit(1);
				}
				if(i2 < 1 || i2 > params.numvnorms || i4 < 1 || i4 > params.numvnorms || i6 < 1 || i6 > params.numvnorms)
				{
					printf("vertex normal referenced in triangle before its instantiation");
					exit(1);
				}
			}
			else if (sscanf(line, "%d %d %d", &i1, &i2, &i3) == 3) 
			{
				if(i1 < 1 || i1 > params.numvertices || i2 < 1 || i2 > params.numvertices || i3 < 1 || i3 > params.numvertices)
				{
					printf("vertex referenced in triangle before its instantiation");
					exit(1);
				}
			} 
			else
			{
				printf("error parsing line %d (triangle)\n", curline);
				exit(1);
			}
		}

		else if (strcmp(keyword, "light") == 0)
		{
			params.numlights++;
			fgets(line, 100, infile);
			if(sscanf(line, "%lf %lf %lf %d %lf", &f1, &f2, &f3, &i1, &f4) < 5)
			{
				printf("error parsing line %d (light)\n", curline);
				exit(1);
			}
			if(i1 != 0 && i1 != 1)
			{
				printf("line %d: light type must be either 0 or 1, currently %d\n", curline, atoi(arg4));
				exit(1);
			}
			if(f4 < 0 || f4 > 1)
			{
				printf("line %d: light intensity must be in range [0,1]\n", curline);
				exit(1);
			}
		}

		else if (strcmp(keyword, "texture") == 0)
		{
			params.numtextures++;
			fgets(line, 100, infile);
			if(sscanf(line, "%s", arg1) < 1)
			{
				printf("error parsing line %d (texture)\n", curline);
				exit(1);
			}
		}

		else
		{
			printf("invalid keyword \"%s\" encountered in or before line %d\n", keyword, curline);
		}
	}

	if(got_imsize < 1)
	{
		printf("\timsize not declared");
		exit(1);
	}
	if(got_eye < 1)
	{
		printf("\teye not declared");
		exit(1);
	}
	if(got_viewdir < 1)
	{
		printf("\tviewdir not declared");
		exit(1);
	}
	if(got_hfov < 1)
	{
		printf("\thfov not declared");
		exit(1);
	}
	if(got_updir < 1)
	{
		printf("\tupdir not declared");
		exit(1);
	}
	if(got_bkgcolor < 1)
	{
		printf("\tbkgcolor not declared");
		exit(1);
	}

	params.mtlcolors = (struct MtlColor*)malloc(params.nummtlcolors*sizeof(struct MtlColor));
	memset(params.mtlcolors, 0, params.nummtlcolors*sizeof(struct MtlColor));
	params.spheres = (struct Sphere*)malloc(params.numspheres*sizeof(struct Sphere));
	memset(params.spheres, 0, params.numspheres*sizeof(struct Sphere));
	params.lights = (struct Light*)malloc(params.numlights*sizeof(struct Light));
	memset(params.lights, 0, params.numlights*sizeof(struct Light));
	params.vertices = (struct PointVec*)malloc(params.numvertices*sizeof(struct PointVec));
	memset(params.vertices, 0, params.numvertices*sizeof(struct PointVec));
	params.triangles = (struct Triangle*)malloc(params.numtriangles*sizeof(struct Triangle));
	memset(params.triangles, 0, params.numtriangles*sizeof(struct Triangle));
	params.vnorms = (struct PointVec*)malloc(params.numvnorms*sizeof(struct PointVec));
	memset(params.vnorms, 0, params.numvnorms*sizeof(struct PointVec));
	params.vtexts = (struct VText*)malloc(params.numvtexts*sizeof(struct VText));
	memset(params.vtexts, 0, params.numvtexts*sizeof(struct VText));
	params.textures = (struct Texture*)malloc(params.numtextures*sizeof(struct Texture));
	memset(params.textures, 0, params.numtextures*sizeof(struct Texture));

	fseek(infile, 0, SEEK_SET);
	int idx_cur_color = -1;
	int idx_cur_texture = -1;
	int idx_cur_sphere = 0;
	int idx_cur_light = 0;
	int idx_cur_vertex = 0;
	int idx_cur_triangle = 0;
	int idx_cur_vnorm = 0;
	int idx_cur_vtext = 0;

	while(fscanf(infile, "%s", keyword) != EOF) 
	{
		if(strcmp(keyword, "mtlcolor") == 0)
		{
			idx_cur_color++;
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %d", &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &i1);
			params.mtlcolors[idx_cur_color].od.r = f1; 
			params.mtlcolors[idx_cur_color].od.g = f2;
			params.mtlcolors[idx_cur_color].od.b = f3;

			params.mtlcolors[idx_cur_color].os.r = f4;
			params.mtlcolors[idx_cur_color].os.g = f5;
			params.mtlcolors[idx_cur_color].os.b = f6;

			params.mtlcolors[idx_cur_color].ka = f7;
			params.mtlcolors[idx_cur_color].kd = f8;
			params.mtlcolors[idx_cur_color].ks = f9;

			params.mtlcolors[idx_cur_color].n = i1;
		}

		else if(strcmp(keyword, "sphere") == 0)
		{
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf %lf %lf", &f1, &f2, &f3, &f4);
			params.spheres[idx_cur_sphere].c.x = f1;
			params.spheres[idx_cur_sphere].c.y = f2;
			params.spheres[idx_cur_sphere].c.z = f3;
			params.spheres[idx_cur_sphere].r = f4;
			params.spheres[idx_cur_sphere].color_idx = idx_cur_color;
			if(idx_cur_texture >= 0)
			{
				params.spheres[idx_cur_sphere].texture_idx = idx_cur_texture;
				params.spheres[idx_cur_sphere].type = SPHERE_TEXTURED;
			}
			else
			{
				params.spheres[idx_cur_sphere].type = SPHERE_UNTEXTURED;
			}
			idx_cur_sphere++;
		}

		else if(strcmp(keyword, "light") == 0)
		{
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf %lf %d %lf", &f1, &f2, &f3, &i1, &f4);
			params.lights[idx_cur_light].locdir.x = f1;
			params.lights[idx_cur_light].locdir.y = f2;
			params.lights[idx_cur_light].locdir.z = f3;
			params.lights[idx_cur_light].type = i1;
			params.lights[idx_cur_light].intensity = f4;
			idx_cur_light++;
		}

		else if(strcmp(keyword, "v") == 0)
		{
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf %lf", &f1, &f2, &f3);
			params.vertices[idx_cur_vertex].x = f1;
			params.vertices[idx_cur_vertex].y = f2;
			params.vertices[idx_cur_vertex].z = f3;
			idx_cur_vertex++;
		}

		else if(strcmp(keyword, "vn") == 0)
		{
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf %lf", &f1, &f2, &f3);
			params.vnorms[idx_cur_vnorm].x = f1;
			params.vnorms[idx_cur_vnorm].y = f2;
			params.vnorms[idx_cur_vnorm].z = f3;
			idx_cur_vnorm++;
		}

		else if(strcmp(keyword, "vt") == 0)
		{
			fgets(line, 100, infile);
			sscanf(line, "%lf %lf", &f1, &f2);
			params.vtexts[idx_cur_vtext].u = f1;
			params.vtexts[idx_cur_vtext].v = f2;
			idx_cur_vtext++;
		}

		else if(strcmp(keyword, "f") == 0)
		{
			fgets(line, 100, infile);
			if (sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d", &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9) == 9)
			{
				params.triangles[idx_cur_triangle].type = TRIANGLE_SMOOTH_TEXTURED;
				params.triangles[idx_cur_triangle].v1_idx = i1-1;
				params.triangles[idx_cur_triangle].v2_idx = i4-1;
				params.triangles[idx_cur_triangle].v3_idx = i7-1;
				params.triangles[idx_cur_triangle].v1_text_idx = i2-1;
				params.triangles[idx_cur_triangle].v2_text_idx = i5-1;
				params.triangles[idx_cur_triangle].v3_text_idx = i8-1;
				params.triangles[idx_cur_triangle].v1_norm_idx = i3-1;
				params.triangles[idx_cur_triangle].v2_norm_idx = i6-1;
				params.triangles[idx_cur_triangle].v3_norm_idx = i9-1;
				params.triangles[idx_cur_triangle].texture_idx = idx_cur_texture;
			}
			else if (sscanf(line, "%d/%d %d/%d %d/%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6)
			{
				params.triangles[idx_cur_triangle].type = TRIANGLE_UNSMOOTH_TEXTURED;
				params.triangles[idx_cur_triangle].v1_idx = i1-1;
				params.triangles[idx_cur_triangle].v2_idx = i3-1;
				params.triangles[idx_cur_triangle].v3_idx = i5-1;
				params.triangles[idx_cur_triangle].v1_text_idx = i2-1;
				params.triangles[idx_cur_triangle].v2_text_idx = i4-1;
				params.triangles[idx_cur_triangle].v3_text_idx = i6-1;
				params.triangles[idx_cur_triangle].texture_idx = idx_cur_texture;
			}
			else if (sscanf(line, "%d//%d %d//%d %d//%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6)
			{
				params.triangles[idx_cur_triangle].type = TRIANGLE_SMOOTH_UNTEXTURED;
				params.triangles[idx_cur_triangle].v1_idx = i1-1;
				params.triangles[idx_cur_triangle].v2_idx = i3-1;
				params.triangles[idx_cur_triangle].v3_idx = i5-1;
				params.triangles[idx_cur_triangle].v1_norm_idx = i2-1;
				params.triangles[idx_cur_triangle].v2_norm_idx = i4-1;
				params.triangles[idx_cur_triangle].v3_norm_idx = i6-1;
			}
			else if (sscanf(line, "%d %d %d", &i1, &i2, &i3) == 3) 
			{
				params.triangles[idx_cur_triangle].type = TRIANGLE_UNSMOOTH_UNTEXTURED;
				params.triangles[idx_cur_triangle].v1_idx = i1-1;
				params.triangles[idx_cur_triangle].v2_idx = i2-1;
				params.triangles[idx_cur_triangle].v3_idx = i3-1;
			} 
			idx_cur_triangle++;
		}

		else if(strcmp(keyword, "texture") == 0)
		{
			idx_cur_texture++;
			fgets(line, 100, infile);
			sscanf(line, "%s", arg1);
			char texture_filepath[80];
			strcpy(texture_filepath, "./texture/");
			strcat(texture_filepath, arg1);
			FILE *textfile = fopen(texture_filepath, "r");
			if(textfile == NULL)
			{
				printf("file does not exist (is it in the ./texture/ directory?)\n");
				exit(1);
			}
			fgets(line, 100, textfile);
			if(sscanf(line, "%s %d %d %d", arg2, &i1, &i2, &i3) < 4)
			{
				printf("error parsing header in texture file \"%s\"\n", arg1);
				exit(1);
			}
			if(strcmp(arg2, "P3"))
			{
				printf("texture file \"%s\" magic number != P3\n", arg1);
			}
			params.textures[idx_cur_texture].width = i1;
			params.textures[idx_cur_texture].height = i2;
			params.textures[idx_cur_texture].pixels = (struct Color**)malloc(i2 * sizeof(struct Color*));
			for(int i = 0; i < i2; i++)
			{
				params.textures[idx_cur_texture].pixels[i] = (struct Color*)malloc(i1 * sizeof(struct Color));
			}
			for(int i = 0; i < i2; i++)
			{
				for(int j = 0; j < i1; j++)
				{
					if(fscanf(textfile, "%lf %lf %lf", &f1, &f2, &f3) < 3)
					{
						printf("error parsing texture file \"%s\\n", arg1);
					}
					params.textures[idx_cur_texture].pixels[i][j].r = f1/i3;
					params.textures[idx_cur_texture].pixels[i][j].g = f2/i3;
					params.textures[idx_cur_texture].pixels[i][j].b = f3/i3;
				}
			}
		}
	}

	if(0)
	{
		printf("imgwidth: %d\n", params.imgwidth);
		printf("imgheight: %d\n", params.imgheight);

		printf("eye_x: %lf\n", params.eye.x);
		printf("eye_y: %lf\n", params.eye.y);
		printf("eye_z: %lf\n", params.eye.z);

		printf("viewdir_x: %lf\n", params.viewdir.x);
		printf("viewdir_y: %lf\n", params.viewdir.y);
		printf("viewdir_z: %lf\n", params.viewdir.z);
	
		printf("fovh: %lf\n", params.fovh);

		printf("updir_x: %lf\n", params.updir.x);
		printf("updir_y: %lf\n", params.updir.y);
		printf("updir_z: %lf\n", params.updir.z);
	
		printf("bkgcolor.r: %lf\n", params.bkgcolor.r);
		printf("bkgcolor.g: %lf\n", params.bkgcolor.g);
		printf("bkgcolor.b: %lf\n", params.bkgcolor.b);
	
		printf("params.nummtlcolors: %d\n", params.nummtlcolors);
		printf("params.numspheres: %d\n", params.numspheres);
		printf("params.numlights: %d\n", params.numlights);
		printf("params.numvertices: %d\n", params.numvertices);
		printf("params.numtriangles: %d\n", params.numtriangles);
		printf("params.numvnorms: %d\n", params.numvnorms);
		printf("params.numvtexts: %d\n", params.numvtexts);
		printf("params.numtextures: %d\n", params.numtextures);

		for(int i = 0; i < params.nummtlcolors; i++)
		{
			printf("params.mtlcolors[%d]:\n", i);
			printf("\tosr: %lf\n", params.mtlcolors[i].os.r);
			printf("\tosg: %lf\n", params.mtlcolors[i].os.g);
			printf("\tosb: %lf\n", params.mtlcolors[i].os.b);
			printf("\todr: %lf\n", params.mtlcolors[i].od.r);
			printf("\todg: %lf\n", params.mtlcolors[i].od.g);
			printf("\todb: %lf\n", params.mtlcolors[i].od.b);
			printf("\tka: %lf\n", params.mtlcolors[i].ka);
			printf("\tkd: %lf\n", params.mtlcolors[i].kd);
			printf("\tks: %lf\n", params.mtlcolors[i].ks);
			printf("\tn: %d\n", params.mtlcolors[i].n);
		}

		for(int i = 0; i < params.numspheres; i++)
		{
			printf("params.spheres[%d]:\n", i);
			printf("\tx_c: %lf\n", params.spheres[i].c.x);
			printf("\ty_c: %lf\n", params.spheres[i].c.y);
			printf("\tz_c: %lf\n", params.spheres[i].c.z);
			printf("\tr: %lf\n", params.spheres[i].r);
			printf("\tcolor_idx: %d\n", params.spheres[i].color_idx);
		}
		for(int i = 0; i < params.numlights; i++)
		{
			printf("params.lights[%d]:\n", i);
			printf("\tx: %lf\n", params.lights[i].locdir.x);
			printf("\ty: %lf\n", params.lights[i].locdir.y);
			printf("\tz: %lf\n", params.lights[i].locdir.z);
			printf("\ttype: %d\n", params.lights[i].type);
			printf("\tintensity: %lf\n", params.lights[i].intensity);
		}
		for(int i = 0; i < params.numvertices; i++)
		{
			printf("params.vertices[%d]:\n", i);
			printf("\tx: %lf\n", params.vertices[i].x);
			printf("\ty: %lf\n", params.vertices[i].y);
			printf("\tz: %lf\n", params.vertices[i].z);
		}
		for(int i = 0; i < params.numtriangles; i++)
		{
			printf("params.triangles[%d]:\n", i);
			printf("\tcolor_idx: %d\n", params.triangles[i].color_idx);
			printf("\ttexture_idx: %d\n", params.triangles[i].texture_idx);
			printf("\ttype: %d\n", params.triangles[i].type);
			printf("\tv1_idx: %d\n", params.triangles[i].v1_idx);
			printf("\tv1_norm_idx: %d\n", params.triangles[i].v1_norm_idx);
			printf("\tv1_text_idx: %d\n", params.triangles[i].v1_text_idx);
			printf("\tv2_idx: %d\n", params.triangles[i].v2_idx);
			printf("\tv2_norm_idx: %d\n", params.triangles[i].v2_norm_idx);
			printf("\tv2_text_idx: %d\n", params.triangles[i].v2_text_idx);
			printf("\tv3_idx: %d\n", params.triangles[i].v3_idx);
			printf("\tv3_norm_idx: %d\n", params.triangles[i].v3_norm_idx);
			printf("\tv3_text_idx: %d\n", params.triangles[i].v3_text_idx);
		}
		for(int i = 0; i < params.numvnorms; i++)
		{
			printf("params.vnorms[%d]:\n",i);
			printf("\tx: %lf\n", params.vnorms[i].x);
			printf("\ty: %lf\n", params.vnorms[i].y);
			printf("\tz: %lf\n", params.vnorms[i].z);
		}
		for(int i = 0; i < params.numvtexts; i++)
		{
			printf("params.vtexts[%d]:\n",i);
			printf("\tu: %lf\n", params.vtexts[i].u);
			printf("\tv: %lf\n", params.vtexts[i].v);
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

	free(params.mtlcolors);
	free(params.spheres);
	free(params.triangles);
	free(params.lights);
	free(params.vertices);
}