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

 [palette.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "palette.h"

struct ko_palette_head
{
	uint16_t magic_number;
	uint16_t version;

	uint8_t colors_no;
	uint8_t format;

	uint8_t unused1;
	uint8_t unused2;
};


/*-----------------------------

 koLoadPalette()
-----------------------------*/
struct ko_palette* koLoadPalette(const char* filename)
{
	struct ko_file* file = NULL;
	struct ko_palette* palette = NULL;
	struct ko_palette_head head;

	koPrint(KO_LOG, "Loading palette '%s'...\n", filename);

	if ((file = koOpenFile(filename, KO_READ, 0)) == NULL)
		goto return_failure;

	if (koReadFile(&head, sizeof(struct ko_palette_head), file) != 0)
		goto return_failure;

	if (head.magic_number != 0x504B) /* "KP" */
	{
		koPrint(KO_LOG, "[Error] File is not an Kobe palette\n");
		goto return_failure;
	}

	if (head.version != 2)
	{
		koPrint(KO_LOG, "[Error] Palette have an unsupported version\n");
		goto return_failure;
	}

	if (head.format != 0)
	{
		koPrint(KO_LOG, "[Error] Palette have an unsupported color format\n");
		goto return_failure;
	}

	koPrint(KO_LOG, " - colors: %u\n", head.colors_no);

	/* Alloc and read */
	palette = koAlloc(sizeof(struct ko_palette) + (sizeof(struct ko_color) * head.colors_no));

	if (palette == NULL)
		goto return_failure;

	palette->colors_no = head.colors_no;
	palette->color = (struct ko_color*)((struct ko_palette*)palette + 1);

	if (koReadFile(palette->color, (sizeof(struct ko_color) * head.colors_no), file) != 0)
		goto return_failure;

	/* Bye! */
	koCloseFile(file);
	return palette;

return_failure:
	koPrint(KO_LOG, "[Error] LoadPalette()\n");

	if (file != NULL)
		koCloseFile(file);

	if (palette != NULL)
		koFree(palette);

	return NULL;
}
