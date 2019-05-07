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

 [indexed-image.c]
 - Alexander Brandt 2018
-----------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

#include "indexed-image.h"


/*-----------------------------

 CreateIndexedImage()
-----------------------------*/
inline struct indexed_image* CreateIndexedImage(int width, int height)
{
	struct indexed_image* new = NULL;

	if ((new = calloc(1, sizeof(struct indexed_image) + (width * height))) != NULL)
	{
		new->width = width;
		new->height = height;
		new->length = width* height;

		// Courtesy grayscale palette
		for (int i = 0; i < 256; i++)
		{
			new->palette[i].r = i;
			new->palette[i].g = i;
			new->palette[i].b = i;
		}
	}

	return new;
}


/*-----------------------------

 DeleteIndexedImage()
-----------------------------*/
inline void DeleteIndexedImage(struct indexed_image* img)
{
	if (img != NULL)
	{
		if (img->region_data != NULL && img->region_data != img->data)
			free(img->region_data);

		free(img);
	}
}


/*-----------------------------

 OpenIndexedImage()
-----------------------------*/
struct indexed_image* OpenIndexedImage(const char* filename)
{
	FILE* file = NULL;
	png_struct* png_ptr = NULL;
	png_info* info_ptr = NULL;
	uint8_t signature[8] = {0};
	struct indexed_image* image = NULL;

	if (filename == NULL)
		return NULL;

	if ((file = fopen(filename, "rb")) == NULL)
	{
		fprintf(stderr, "[Error] Can not open '%s'\n", filename);
		goto return_failure;
	}

	if (fread(&signature, 1, 8, file) != 8 || png_sig_cmp(signature, 0, 8) != 0)
	{
		fprintf(stderr, "[Error] Not a png file\n");
		goto return_failure;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);

	if (png_ptr == NULL || info_ptr == NULL)
		goto return_failure;

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);

	// Read information
	int32_t depth = 0;
	int32_t color_type = 0;
	uint32_t width = 0;
	uint32_t height = 0;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &color_type, NULL, NULL, NULL);

	if (color_type != PNG_COLOR_TYPE_PALETTE || depth != 8)
		goto return_failure;

	if ((image = CreateIndexedImage(width, height)) == NULL)
		goto return_failure;

	// Read palette
	png_color* raw_palette = NULL;
	int32_t colors_no = 0;

	png_get_PLTE(png_ptr, info_ptr, &raw_palette, &colors_no);

	for (int32_t i = 0; i < colors_no && i < 256; i++)
	{
		image->palette[i].r = (raw_palette + i)->red;
		image->palette[i].g = (raw_palette + i)->green;
		image->palette[i].b = (raw_palette + i)->blue;
	}

	image->colors = colors_no;

	// Read data
	for (uint8_t* dest = image->data; dest < (image->data + image->length); dest += image->width)
		png_read_row(png_ptr, dest, NULL);

	// Bye!
	fclose(file);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return image;

return_failure:

	if (image != NULL)
		DeleteIndexedImage(image);

	if (file != NULL)
		fclose(file);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return NULL;
}


/*-----------------------------

 SaveIndexedImage()
-----------------------------*/
inline int SaveIndexedImage(const struct indexed_image* img, const char* filename)
{
	FILE* file = NULL;
	png_struct* png_ptr = NULL;
	png_info* info_ptr = NULL;

	if (img == NULL || filename == NULL)
		return 1;

	if ((file = fopen(filename, "wb")) == NULL)
		return 1;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);

	if (png_ptr == NULL || info_ptr == NULL)
	{
		fclose(file);
		return 1;
	}

	png_init_io(png_ptr, file);

	// Write information
	for (int i = 0; i < 256; i++)
	{
		// Using a custom palette?
		if (img->palette[i].r != i || img->palette[i].g != i || img->palette[i].b != i)
		{
			printf("TODO TODO TODO\n");

			png_set_IHDR(png_ptr, info_ptr, img->width, img->height, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
			png_set_PLTE(png_ptr, info_ptr, ((png_color*)img->palette), 256);

			goto ignore_grayscale;
		}
	}

	png_set_IHDR(png_ptr, info_ptr, img->width, img->height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

ignore_grayscale:
	png_write_info(png_ptr, info_ptr);

	// Write data
	const uint8_t* img_end = &img->data[img->width * img->height];

	for (const uint8_t* row = img->data; row < img_end; row += img->width)
		png_write_row(png_ptr, row);

	// Bye!
	png_write_end(png_ptr, info_ptr);

	fclose(file);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return 0;
}


/*-----------------------------

 SetIndexedImageRegion()
-----------------------------*/
int SetIndexedImageRegion(struct indexed_image* img, int x, int y, int width, int height)
{
	uint8_t* temp_data = NULL;

	if (x == 0 && y == 0 && width == img->width && height == img->height)
	{
		img->region_data = img->data;
	}
	else if ((width * height) > img->region_length)
	{
		if (img->region_data == img->data)
			img->region_data = NULL;

		temp_data = img->region_data;

		if ((img->region_data = realloc(img->region_data, width * height)) == NULL)
		{
			img->region_data = temp_data;
			return 1;
		}
	}

	img->region_width = width;
	img->region_height = height;
	img->region_length = (width * height);

	size_t s = (img->width * y) + x;
	size_t d = 0;

	for(size_t row = 0; row < height; row++)
	{
		memcpy(&img->region_data[d], &img->data[s], img->region_width);

		s += img->width;
		d += img->region_width;
	}

	return 0;
}
