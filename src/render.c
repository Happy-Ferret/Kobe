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

 [render.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "render.h"

#define SEQUENCER_REGS 0x03C4
#define SEQUENCER_REGS_DATA 0x03C5

#define CRT_REGS 0x03D4
#define CRT_REGS_DATA 0x03D5


static uint8_t s_prev_mode;
static struct ko_render_state s_internal_state;
static struct ko_render_state s_state;


static uint16_t s_logical_rows[240];


/*-----------------------------

 koBackground()
-----------------------------*/
static void koBackground(uint32_t KO_FAR* data, int16_t h, uint8_t column_start, uint8_t column_end)
{
	uint8_t plane = 0;
	uint8_t row = 0;
	uint8_t col = 0;

	uint8_t KO_FAR* vga_offset = NULL;
	uint32_t KO_FAR* data_offset = (data) + (h * (uint16_t)column_start);

	koDosPortOut(0x03C4, 0x02); /* Set plane, 0x03C4 = Sequence register */

	for (col = column_start; col < column_end; col++)
	{
		for (plane = 0; plane < 4; plane++)
		{
			koDosPortOut(0x03C5, 0x01 << plane); /* Set plane, 0x03C5 = Sequence data */
			vga_offset = (uint8_t KO_FAR*)(0xA0000000) + (col % s_logical_rows[1]);

			for (row = 0; row < h; row += 8)
			{
				*vga_offset = *data_offset;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 8;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 16;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 24;
				vga_offset += s_logical_rows[1];

				data_offset++;

				/* Unrolled loop */
				*vga_offset = *data_offset;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 8;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 16;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 24;
				vga_offset += s_logical_rows[1];

				data_offset ++;
			}
		}
	}
}


/*-----------------------------

 koHugeBackground()
-----------------------------*/
static void koHugeBackground(uint32_t KO_HUGE* data, uint8_t h, uint8_t column_start, uint8_t column_end)
{
	uint8_t plane = 0;
	uint8_t row = 0;
	uint8_t col = 0;

	uint8_t KO_FAR* vga_offset = NULL;
	uint32_t KO_HUGE* data_offset = (data) + (h * (uint16_t)column_start);

	koDosPortOut(0x03C4, 0x02); /* Set plane, 0x03C4 = Sequence register */

	for (col = column_start; col < column_end; col++)
	{
		for (plane = 0; plane < 4; plane++)
		{
			koDosPortOut(0x03C5, 0x01 << plane); /* Set plane, 0x03C5 = Sequence data */
			vga_offset = (uint8_t KO_FAR*)(0xA0000000) + (col % s_logical_rows[1]);

			if (col >= s_logical_rows[1])
				vga_offset += s_logical_rows[1];

			for (row = 0; row < h; row += 8)
			{
				*vga_offset = *data_offset;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 8;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 16;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 24;
				vga_offset += s_logical_rows[1];

				data_offset++;

				/* Unrolled loop */
				*vga_offset = *data_offset;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 8;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 16;
				vga_offset += s_logical_rows[1];

				*vga_offset = *data_offset >> 24;
				vga_offset += s_logical_rows[1];

				data_offset ++;
			}
		}
	}
}


/*-----------------------------

 koRectangle()
-----------------------------*/
#if 0
static void koRectangle(uint8_t color, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
	uint8_t plane = 0;
	uint8_t row = 0;

	uint8_t KO_FAR* vga_offset = NULL;

	koDosPortOut(0x03C4, 0x02); /* Set plane, 0x03C4 = Sequence register */

	for (plane = 0; plane < 4; plane++)
	{
		koDosPortOut(0x03C5, 0x01 << plane); /* Set plane, 0x03C5 = Sequence data */
		vga_offset = (uint8_t KO_FAR*)(0xA0000000) + (((plane + x) >> 2) + s_logical_rows[y]);

		for (row = 0; row < height; row++)
		{
			koDosFarMemorySet(vga_offset, color, width >> 2);
			vga_offset += s_logical_rows[1];
		}
	}
}
#endif


/*-----------------------------

 koImage()
-----------------------------*/
static void koImage(uint8_t KO_FAR* data, int16_t width, int16_t height, int16_t x, int16_t y)
{
	uint8_t plane = 0;
	uint8_t row = 0;

	uint8_t KO_FAR* vga_offset = NULL;

	koDosPortOut(0x03C4, 0x02); /* Set plane, 0x03C4 = Sequence register */

	for (plane = 0; plane < 4; plane++)
	{
		koDosPortOut(0x03C5, 0x01 << plane); /* Set plane, 0x03C5 = Sequence data */
		vga_offset = (uint8_t KO_FAR*)(0xA0000000) + (((plane + x) >> 2) + s_logical_rows[y]);

		for (row = 0; row < height; row++)
		{
			koDosFarMemoryCopy(vga_offset, data, width >> 2);

			data += width >> 2;
			vga_offset += s_logical_rows[1];
		}
	}
}


/*-----------------------------

 koRenderInit()
-----------------------------*/
unsigned koRenderInit(const char* palette_filename, unsigned resolution_x)
{
	union ko_regs reg;
	int16_t i = 0;
	struct ko_palette* pal = NULL;

	koPrint(KO_LOG, "initializing render module...\n");

	if (resolution_x > 960 || resolution_x < 320)
		return 1;

	/*
	 Get current video mode (Int 10/AH=0Fh)
	 - http://www.ctyme.com/intr/rb-0108.htm
	*/
	reg.h.ah = 0x0F;
	koDosInterrupt(0x10, &reg, &reg);
	s_prev_mode = reg.h.al; /* al = video mode */

	koPrint(KO_LOG, " - previous video mode: 0x%02X\n", s_prev_mode);

	/*
	 Set VGA mode 13 (Int 10/AH=00h)
	 - http://www.ctyme.com/intr/rb-0069.htm
	*/
	reg.h.ah = 0x00;
	reg.h.al = 0x13; /* al = video mode */
	koDosInterrupt(0x10, &reg, &reg);

	/*
	 Get current video mode, again, to check (Int 10/AH=0Fh)
	 - Looks like different chips, return different values when
	   the mode is set (above section). So we need to call this
	   interruptor again.
	*/
	reg.h.ah = 0x0F;
	koDosInterrupt(0x10, &reg, &reg);

	if (reg.h.al != 0x13) /* al = video mode */
	{
		koPrint(KO_LOG | KO_COUT, "[error] | you need a VGA adapter\n");
		goto return_error;
	}

	/*
	 320x240 resolution
	 - Note the 'write protect' registers, needed for indexes 00h-07h
	 - http://www.gamers.org/dEngine/rsc/pcgpe-1.0/modex.txt
	 - http://www.osdever.net/FreeVGA/vga/crtcreg.htm
	 - http://www.osdever.net/FreeVGA/vga/extreg.htm
	*/
	{
		/*
		 The next out call is extremely important... and tricky.
		 - Sets the vertical and horizontal sync polarity (?).
		 - And set the "CRT Controller Registers" to address 0x03D4 / D5.

		 All the out calls in the engine write to that address, check
		 the macro "CRT_REGS" and "CRT_REGS_DATA"
		 */
		koDosPortOut(0x03C2, 0xE3); /* 0x03C2 = miscellaneous output register */

		koDosPortOut(CRT_REGS, 0x11);      /* 0x11 = vertical retrace end register */
		koDosPortOut(CRT_REGS_DATA, 0x2C); /* turn off write protect */

		koDosPortOut(CRT_REGS, 0x06); /* 0x06 = vertical total register */
		koDosPortOut(CRT_REGS_DATA, 0x0D);

		koDosPortOut(CRT_REGS, 0x07); /* 0x07 = overflow register */
		koDosPortOut(CRT_REGS_DATA, 0x3E);

		koDosPortOut(CRT_REGS, 0x10); /* 0x10 = vertical retrace start register */
		koDosPortOut(CRT_REGS_DATA, 0xEA);

		koDosPortOut(CRT_REGS, 0x11);      /* 0x11 = vertical retrace end register */
		koDosPortOut(CRT_REGS_DATA, 0xAC); /* turn on write protect */

		koDosPortOut(CRT_REGS, 0x12); /* 0x12 = vertical display end register */
		koDosPortOut(CRT_REGS_DATA, 0xDF);

		koDosPortOut(CRT_REGS, 0x15); /* 0x15 = start vertical blanking register */
		koDosPortOut(CRT_REGS_DATA, 0xE7);

		koDosPortOut(CRT_REGS, 0x16); /* 0x16 = end vertical blanking register */
		koDosPortOut(CRT_REGS_DATA, 0x06);
	}

	/*
	 Unchainted mode (256k VGA memory access)
	 - This section is independent from the above one, except for
	   the CRT_REGS out's.
	 - http://www.gamers.org/dEngine/rsc/pcgpe-1.0/modex.txt
	 - http://www.osdever.net/FreeVGA/vga/seqreg.htm
	*/
	{
		/* Turn off the chain-4 bit (chained mode) */
		koDosPortOut(SEQUENCER_REGS, 0x04); /* 0x04 = sequencer memory mode register */
		koDosPortOut(SEQUENCER_REGS_DATA, 0x06);

		/* Turn off doubleword mode (long mode) */
		koDosPortOut(CRT_REGS, 0x14); /* 0x14 = underline location register */
		koDosPortOut(CRT_REGS_DATA, 0x00);

		/* Turn off word mode (turn on byte mode) */
		koDosPortOut(CRT_REGS, 0x17); /* 0x17 = CRTC mode control register */
		koDosPortOut(CRT_REGS_DATA, 0xE3);
	}

	/*
	 Logical Width (offscreen total width) (40 = default, one screen)
	 - http://qzx.com/pc-gpe/scroll.txt
	 - http://www.osdever.net/FreeVGA/vga/crtcreg.htm
	*/
	koDosPortOut(CRT_REGS, 0x13);                   /* 0x13 = offset register */
	koDosPortOut(CRT_REGS_DATA, resolution_x / 8); /* 320px = 40 columns */

	for (i = 0; i < 240; i++)
		s_logical_rows[i] = i * ((resolution_x / 8) * 2);

	s_internal_state.resolution_x = resolution_x;
	koPrint(KO_LOG, " - logical width: %u (%u px)\n", (resolution_x / 8), s_internal_state.resolution_x);

	/* Palette */
	if (palette_filename != NULL)
	{
		if ((pal = koLoadPalette(palette_filename)) == NULL)
			koPrint(KO_LOG, "[warning] | koLoadPalette() ['%s']\n", palette_filename);

		koRenderPalette(pal);
	}

	/* Return */
	koDeletePalette(pal);
	return 0;

return_error:
	if (pal != NULL)
		koDeletePalette(pal);

	s_internal_state.vga_error = true;
	return 1;
}


/*-----------------------------

 koRenderStop()
-----------------------------*/
void koRenderStop()
{
	union ko_regs reg;

	koPrint(KO_LOG, "stopping render module...\n");

	/*
	 Restore VGA mode (Int 10/AH=00h)
	 - http://www.ctyme.com/intr/rb-0069.htm
	*/
	if (s_internal_state.vga_error == false)
	{
		reg.h.ah = 0x00;
		reg.h.al = s_prev_mode;
		koDosInterrupt(0x10, &reg, &reg);
	}

	memset(&s_internal_state, 0, sizeof(struct ko_render_state));
	memset(&s_state, 0, sizeof(struct ko_render_state));
}


/*-----------------------------

 koRenderPalette()
-----------------------------*/
void koRenderPalette(const struct ko_palette* palette)
{
	int i = 0;

	if (palette == NULL)
		return;

	koDosPortOut(0x03C8, 0); /* TODO: documentation */

	for (i = 0; i <= (palette->colors_no) && i <= 254; i++)
	{
		koDosPortOut(0x03C9, (palette->color[i].r * 63) / 255);
		koDosPortOut(0x03C9, (palette->color[i].g * 63) / 255);
		koDosPortOut(0x03C9, (palette->color[i].b * 63) / 255);
	}

	#if 0
	/* In case of a incomplete palette */
	for (; i <= 254; i++)
	{
		koDosPortOut(0x03C9, (255 * 63) / 255);
		koDosPortOut(0x03C9, (0 * 63) / 255);
		koDosPortOut(0x03C9, (254 * 63) / 255);
	}

	/* Hardcoded 255 color (intended has transparent) */
	koDosPortOut(0x03C9, (255 * 63) / 255);
	koDosPortOut(0x03C9, (0 * 63) / 255);
	koDosPortOut(0x03C9, (254 * 63) / 255);
	#endif

	koDosPortOut(0x03C4, 0x02); /* TODO: documentation */
	koDosPortOut(0x03C5, 0x0F); /* TODO: documentation */
}


/*-----------------------------

 koRenderClean()
-----------------------------*/
void koRenderClean(uint8_t color)
{
	koDosPortOut(SEQUENCER_REGS, 0x02);
	koDosPortOut(SEQUENCER_REGS_DATA, 0x0F);
	koDosFarMemorySet((uint8_t KO_FAR*)(0xA0000000), color, 0xFFFF);
}


/*-----------------------------

 koRenderBackground()
-----------------------------*/
void koRenderBackground(const struct ko_image* image) { s_internal_state.background = image; }


/*-----------------------------

 koRenderOffset()
-----------------------------*/
void koRenderOffset(int16_t offset)
{
	if (offset < 0)
		offset = 0;
	else if (offset > s_internal_state.resolution_x - 320)
		offset = s_internal_state.resolution_x - 320;

	s_internal_state.offset = offset;
	s_internal_state.offset_rows = s_internal_state.offset >> 2;
}


/*-----------------------------

 koRenderSpriteCreate()
-----------------------------*/
int koRenderSpriteCreate(const struct ko_image* image, int16_t x, int16_t y)
{
	int i = 0;

	for (i = 0; i < KO_MAX_SPRITES; i++)
	{
		if (s_internal_state.sprite[i].image == NULL)
		{
			s_internal_state.sprite[i].image = image;
			s_internal_state.sprite[i].x = x;
			s_internal_state.sprite[i].y = y;
			s_internal_state.sprite[i].updated = false;
			return i;
		}
	}

	return -1;
}


/*-----------------------------

 koRenderUpdate()
-----------------------------*/
#define s_offset(offset)\
	koDosPortOut(CRT_REGS, 0x0D);\
	koDosPortOut(CRT_REGS_DATA, (uint8_t)(offset & 0x00FF));\
	koDosPortOut(CRT_REGS, 0x0C);\
	koDosPortOut(CRT_REGS_DATA, (uint8_t)((offset & 0xFF00) >> 8));

const struct ko_render_state* koRenderUpdate()
{
	bool changes = false;
	int16_t relative_offset = 0;
	int16_t distance = 0;
	struct ko_sprite* sprite = NULL;

	/* Entry background changed */
	if (s_internal_state.background != s_state.background)
	{
		changes = true;

		/*s_offset(32768);*/ /* TODO: hardcoded, and probably a wrong address */

		if (s_internal_state.background->data_huge != NULL)
		{
			koHugeBackground(s_internal_state.background->data_huge, s_internal_state.background->height,
			                 s_internal_state.offset_rows, s_internal_state.offset_rows + 80);
		}
		else
		{
			koBackground((uint32_t KO_FAR*)s_internal_state.background->data[0], s_internal_state.background->height,
			             s_internal_state.offset_rows, s_internal_state.offset_rows + 80);
		}
	}

	/* Offset changed */
	else if (s_internal_state.offset != s_state.offset)
	{
		changes = true;
		relative_offset = s_internal_state.offset_rows - s_state.offset_rows;

		if (s_internal_state.background->data_huge != NULL)
		{
			if (relative_offset > 0)
				koHugeBackground(s_internal_state.background->data_huge, s_internal_state.background->height,
				                 s_state.offset_rows + 80, s_state.offset_rows + 80 + relative_offset);
			else if (relative_offset < 0)
				koHugeBackground(s_internal_state.background->data_huge, s_internal_state.background->height,
				                 s_state.offset_rows + relative_offset, s_state.offset_rows);
		}
	}

	/* Changes on sprites */
	for (sprite = s_internal_state.sprite; sprite < (s_internal_state.sprite + KO_MAX_SPRITES); sprite++)
	{
		if (sprite->image == NULL)
			continue;

		/* Visibility check, relative to the center of screen */
		distance =
		    abs(sprite->x + (sprite->image->width >> 1) - s_internal_state.offset - 160) + (sprite->image->width >> 1);

		if ((distance) < 160)
		{
			/* Is in the border of screen? */
			if (distance >= (128 - 4) && (distance / 8) != sprite->prev_step)
			{
				sprite->prev_step = distance / 8;
				sprite->updated = false;
			}

			sprite->visible = true;
		}
		else
		{
			sprite->visible = false;
			sprite->updated = false;
		}

		/* Update? */
		if (changes == false || sprite->visible == false || sprite->updated == true)
			continue;

		sprite->updated = true;

		/* Is a pop in or out? */
		if ((sprite->x - (sprite->image->width >> 1)) > s_internal_state.offset + 160)
		{
			if (relative_offset < 0)
				goto pop_out;
		}
		else
		{
			if (relative_offset > 0)
				goto pop_out;
		}

		if (relative_offset == 0)
		{
			/* TODO */
		}

		/* Pop In */
		if (sprite->prev_step <= 16)
			koImage(sprite->image->data[1], sprite->image->width, sprite->image->height, sprite->x, sprite->y);

		continue;

		/* Pop Out */
	pop_out:
		if (sprite->prev_step >= 19)
			koImage(sprite->image->data[0], sprite->image->width, sprite->image->height, sprite->x, sprite->y);
	}

	/* Return! */
	if (changes == true)
	{
		memcpy(&s_state, &s_internal_state, sizeof(struct ko_render_state));
		s_offset(s_internal_state.offset_rows);
	}

	return &s_state;
}
