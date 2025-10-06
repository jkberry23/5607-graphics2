import sys
import os

# check for correct calling convention
if len(sys.argv) != 2:
	print("usage: python3 ppm_generator.py <filename>")
	exit(0)

filename_in = sys.argv[1]
filename_out = filename_in[:-4] + ".ppm"
input_dir_path = "./input/"
output_dir_path = "./output/"

# check if input file exists
found_input_file = 0
for input_file in os.listdir(input_dir_path):
	if input_file == filename_in:
		found_input_file = 1

if found_input_file == 0:
	print("input file not found in input directory")
	exit(0)

# read each line from input file into "lines" list
infile = open(input_dir_path+filename_in, 'r')
lines = infile.readlines()

infile.close()

# check that input file only contains one line
if len(lines) != 1:
	print("input file formatted incorrectly, should be:\nimsize <width> <height>")
	exit(0)

size_info = lines[0].split()

# check that the input file's line is formatted correctly
if len(size_info) != 3 or size_info[0] != "imsize":
	print("input file formatted incorrectly, should be:\nimsize <width> <height>")
	exit(0)

width = int(size_info[1])
height = int(size_info[2])

if width < 1:
	print("width must be >= 1")
	exit(0)

if height < 1:
	print("height must be >= 1")
	exit(0)

outfile = open(output_dir_path + filename_out, "w")

# write image info to output file
outfile.write("P3\n")
outfile.write(str(width) + " " + str(height) + "\n255")

# write pixels to output file; each line contains rgb values of 5 pixels
for i in range(height):
	for j in range(width):
		if j % 5 == 0:
			outfile.write("\n")
		else:
			outfile.write(" ")
		outfile.write("0 0 0")

outfile.close()



