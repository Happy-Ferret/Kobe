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

 [kpalette.c]
 - Alexander Brandt 2018
-----------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "indexed-image.h"
#include "kpalette.h"


struct kpalette_head
{
	uint16_t magic_number;
	uint16_t version;

	uint8_t colors;
	uint8_t format;

	uint8_t unused1;
	uint8_t unused2;
};


/*-----------------------------

 ConvertToKpalette()
-----------------------------*/
int ConvertToKpalette(const char* input_filename, const char* output_filename)
{
	struct kpalette_head head = {0};
	struct indexed_image* image = NULL;
	FILE* file = NULL;

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

	head.magic_number = 0x504B; // "KP"
	head.version = 2;
	head.colors = image->colors;
	head.format = 0; // RGBX

	if (fwrite(&head, sizeof(struct kpalette_head), 1, file) != 1)
	{
		fprintf(stderr, "[Error] While writting '%s' head\n", output_filename);
		goto return_failure;
	}

	if (fwrite(image->palette, sizeof(struct color), image->colors, file) != image->colors)
	{
		fprintf(stderr, "[Error] While writting '%s' data\n", output_filename);
		goto return_failure;
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
