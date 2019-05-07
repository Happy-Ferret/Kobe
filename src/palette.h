/*-----------------------------

 [palette.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_PALETTE_H
#define KO_PALETTE_H

	#include "kobe.h"
	#include "util.h"

	struct ko_color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t padding;
	};

	struct ko_palette
	{
		int colors_no;
		struct ko_color* color;
	};

	struct ko_palette* koLoadPalette(const char* filename);
	#define koDeletePalette(palette) (koFree(palette))

#endif
