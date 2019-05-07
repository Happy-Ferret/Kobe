/*-----------------------------

 [render.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_RENDER_H
#define KO_RENDER_H

	#include "kobe.h"
	#include "util.h"

	#include "time.h"
	#include "image.h"
	#include "palette.h"

	#define KO_MAX_SPRITES 8

	struct ko_sprite
	{
		int16_t x;
		int16_t y;

		bool visible;
		bool updated;

		int8_t prev_step;

		const struct ko_image* image;
	};

	struct ko_render_state
	{
		struct ko_sprite sprite[KO_MAX_SPRITES];
		bool vga_error;

		const struct ko_image* background;

		uint16_t resolution_x;
		int16_t offset;
		int16_t offset_rows;
	};

	unsigned koRenderInit(const char* palette_filename, unsigned resolution_width);
	void koRenderStop();

	const struct ko_render_state* koRenderUpdate();

	void koRenderPalette(const struct ko_palette* palette);
	void koRenderClean(uint8_t color);
	void koRenderBackground(const struct ko_image* image);

	void koRenderOffset(int16_t offset);

	int koRenderSpriteCreate(const struct ko_image* image, int16_t x, int16_t y);

#endif
