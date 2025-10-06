#ifndef RAYTRACER1C_IO_H
#define RAYTRACER1C_IO_H

#include <stdio.h>
#include <string.h>
#include "raytracer1c.h"

void read_input_file(const char *filename);

void write_output_file(const char *filename, struct Color outpixels[params.imgheight][params.imgwidth]);

#endif // RAYTRACER1C_IO_H
