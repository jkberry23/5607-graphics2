#ifndef RAYTRACER1D_IO_H
#define RAYTRACER1D_IO_H

#include <stdio.h>
#include <string.h>
#include "raytracer1d.h"

void read_input_file(const char *filename);

void write_output_file(const char *filename, struct Color outpixels[params.imgheight][params.imgwidth]);

#endif // RAYTRACER1D_IO_H
