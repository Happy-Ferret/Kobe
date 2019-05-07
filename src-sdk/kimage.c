/*-----------------------------

MIT License

Copyright (c) 2018 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [kimage.c]
 - Alexander Brandt 2018
-----------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "indexed-image.h"
#include "kimage.h"


struct kimage_head
{
	uint16_t magic_number;
	uint16_t version;

	uint16_t width;
	uint16_t height;

	struct
	{
		uint8_t arrange : 2;
		uint8_t compressed : 1;

	} format;

	uint8_t frames;

	uint8_t unused1;
	uint8_t unused2;

	int16_t offset_x;
	int16_t offset_y;
};


static inline int s_linear(struct indexed_image* image, int rx, int ry, int rw, int rh, FILE* file)
{
	if (SetIndexedImageRegion(image, rx, ry, rw, rh) != 0)
		return 1;

	if (fwrite(image->region_data, 1, (rw * rh), file) != (size_t)(rw * rh))
		return 1;

	return 0;
}


static inline int s_columns(struct indexed_image* image, int rx, int ry, int rw, int rh, FILE* file)
{
	if (SetIndexedImageRegion(image, rx, ry, rw, rh) != 0)
		return 1;

	for (int col = 0; col < image->region_width; col++)
	{
		for (int row = 0; row < image->region_height; row++)
		{
			if (fwrite(&image->region_data[col + row * image->region_width], 1, 1, file) != 1)
				return 1;
		}
	}

	return 0;
}


static inline int s_planes(struct indexed_image* image, int rx, int ry, int rw, int rh, FILE* file)
{
	if (SetIndexedImageRegion(image, rx, ry, rw, rh) != 0)
		return 1;

	for (int page = 0; page < 4; page++)
	{
		for (int px = page; px < (image->region_width * image->region_height); px += 4)
		{
			if (fwrite(&image->region_data[px], 1, 1, file) != 1)
				return 1;
		}
	}

	return 0;
}


/*-----------------------------

 ConvertToKimage()
-----------------------------*/
int ConvertToKimage(const char* input_filename, const char* output_filename, enum arrange arrange, int frame_w,
                    int frame_h)
{
	struct kimage_head head = {0};
	struct indexed_image* image = NULL;
	FILE* file = NULL;

	int frames_no = 1;

	if ((file = fopen(output_filename, "wb")) == NULL)
	{
		fprintf(stderr, "[Error] Opening '%s'\n", output_filename);
		goto return_failure;
	}

	if ((image = OpenIndexedImage(input_filename)) == NULL)
	{
		fprintf(stderr, "[Error] Opening '%s'\n", input_filename);
		goto return_failure;
	}

	// Checks
	if (frame_w > image->width || frame_h > image->height)
	{
		fprintf(stderr, "[Error] It is complicated to animate an image of %ix%i px with frames of %ix%i px\n",
		        image->width, image->height, frame_w, frame_h);
		return 1;
	}

	if (frame_w == 0 || frame_h == 0)
	{
		frame_w = image->width; // No animation
		frame_h = image->height;
	}

	frames_no = (image->width / frame_w) * (image->height / frame_h);

	if ((frame_w % 4) != 0 || (frame_h % 4) != 0)
	{
		fprintf(stderr, "[Error] %s dimensions are not multiple of 4\n", (frames_no > 1) ? "Frames" : "Image");
		return 1;
	}

	// Head
	head.magic_number = 0x494B; // "KI"
	head.version = 2;
	head.width = frame_w;
	head.height = frame_h;
	head.format.arrange = arrange;
	head.frames = frames_no;

	if (fwrite(&head, sizeof(struct kimage_head), 1, file) != 1)
	{
		fprintf(stderr, "[Error] While writting '%s' head\n", output_filename);
		goto return_failure;
	}

#if 1
	printf("%s - > %s:\n", input_filename, output_filename);
	printf(" - size: %ix%i px, %i frame(s)\n", head.width, head.height, head.frames);

	if (head.frames != 1)
		printf(" - original size: %ix%i px\n", image->width, image->height);

	printf(" - arrange: %i\n", head.format.arrange);
#endif

	// Data
	for (int i = 0, frame_x = 0, frame_y = 0; i < frames_no; i++, frame_x += frame_w)
	{
		if ((frame_x + frame_w) > image->width)
		{
			frame_x = 0;
			frame_y += frame_h;
		}

#if 1
		if (head.frames != 1)
			printf(" - frame %03i: pos: %i,%i, size: %ix%i\n", i, frame_x, frame_y, frame_w, frame_h);
#endif

		if (arrange == LINEAR)
		{
			if (s_linear(image, frame_x, frame_y, frame_w, frame_h, file) != 0)
				goto return_failure;
		}
		else if (arrange == COLUMNS)
		{
			if (s_columns(image, frame_x, frame_y, frame_w, frame_h, file) != 0)
				goto return_failure;
		}
		else if (arrange == PLANES)
		{
			if (s_planes(image, frame_x, frame_y, frame_w, frame_h, file) != 0)
				goto return_failure;
		}
	}

	// Bye!
	fclose(file);
	DeleteIndexedImage(image);
	return 0;

return_failure:
	if (file != NULL)
		fclose(file);

	if (image != NULL)
		DeleteIndexedImage(image);

	return 1;
}
