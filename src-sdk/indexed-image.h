/*-----------------------------

 [indexed-image.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef INDEXED_IMAGE_H
#define INDEXED_IMAGE_H

	#include <stdint.h>

	struct color // Same as 'png_color'
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t padding;
	};

	struct indexed_image
	{
		int width;
		int height;
		int length;

		uint8_t colors;
		struct color palette[256];

		int region_width;
		int region_height;
		int region_length;

		uint8_t* region_data;
		uint8_t data[];
	};

	struct indexed_image* CreateIndexedImage(int width, int height);
	void DeleteIndexedImage(struct indexed_image* img);

	struct indexed_image* OpenIndexedImage(const char* filename);
	int SaveIndexedImage(const struct indexed_image* img, const char* filename);

	int SetIndexedImageRegion(struct indexed_image* img, int x, int y, int width, int height);

#endif
