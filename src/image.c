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

 [image.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "image.h"
#include "util.h"

enum ko_image_arrange
{
	KO_LINEAR = 0x00,
	KO_COLUMNS = 0x01,
	KO_PLANES = 0x02,
	KO_UNUSED = 0x03
};

struct ko_image_format
{
	uint8_t arrange : 2;
	uint8_t compressed : 1;
};

struct ko_image_head
{
	uint16_t magic_number;
	uint16_t version;

	uint16_t width;
	uint16_t height;

	struct ko_image_format format;

	uint8_t frames_no;

	uint8_t unused1;
	uint8_t unused2;

	int16_t offset_x;
	int16_t offset_y;
};


/*-----------------------------

 koImageLoad()
-----------------------------*/
struct ko_image* koImageLoad(const char* filename, uint8_t KO_HUGE* hdest)
{
	struct ko_image_head head;
	struct ko_image* image = NULL;
	struct ko_file* file = 0;

	int i = 0;
	uint32_t to_read = 0;

	koPrint(KO_LOG, "Loading image '%s'...\n", filename);

	if ((file = koOpenFile(filename, KO_READ, 0)) == NULL)
		goto return_failure;

	if (koReadFile(&head, sizeof(struct ko_image_head), file) != 0)
		goto return_failure;

	if (head.magic_number != 0x494B) /* "KI" */
	{
		koPrint(KO_LOG, "[Error] File is not an Kobe image\n");
		goto return_failure;
	}

	if (head.version != 2)
	{
		koPrint(KO_LOG, "[Error] Image have an unsupported version\n");
		goto return_failure;
	}

	koPrint(KO_LOG, " - width: %u px\n", head.width);
	koPrint(KO_LOG, " - height: %u px\n", head.height);
	koPrint(KO_LOG, " - frames: %u\n", head.frames_no);

	/* Alloc and read */
	if ((image = koAlloc(sizeof(struct ko_image) + sizeof(uint8_t KO_FAR*) * head.frames_no)) == NULL)
		goto return_failure;

	image->width = head.width;
	image->height = head.height;
	image->length = (uint32_t)head.width * (uint32_t)head.height;
	image->frames_no = head.frames_no;
	image->data = (uint8_t KO_FAR**)((struct ko_image*)image + 1);
	image->data_huge = (uint32_t KO_HUGE*)hdest;

	if (image->length < (uint32_t)UINT16_MAX)
	{
		for (i = 0; i < image->frames_no; i++)
		{
			if ((image->data[i] = koFarAlloc(image->length)) == NULL)
				goto return_failure;

			koPrint(KO_LOG, " - frame %u: %04X:%04X-F\n", i, koDosPointerSegment(image->data[i]),
			        koDosPointerOffset(image->data[i]));

			if (koReadFileFar(image->data[i], image->length, file) != 0)
				goto return_failure;
		}
	}
	else
	{
		if (image->data_huge == NULL)
		{
			koPrint(KO_LOG, "[Error] Huge pointer not provided\n");
			goto return_failure;
		}

		/* First frame only */
		koPrint(KO_LOG, " - frame 0: %04X:%04X-H\n", koDosPointerSegment(image->data_huge),
		        koDosPointerOffset(image->data_huge));
		to_read = image->length;

		while (to_read > (uint32_t)UINT16_MAX)
		{
			koReadFileFar(hdest, UINT16_MAX, file);

			to_read -= (uint32_t)UINT16_MAX;
			hdest += (uint32_t)UINT16_MAX;
		}

		koReadFileFar(hdest, to_read, file);
	}

	/* Bye! */
	koCloseFile(file);
	return image;

return_failure:
	koPrint(KO_LOG, "[Error] LoadImage()\n");

	if (file != NULL)
		koCloseFile(file);

	if (image != NULL)
		koImageFree(image);

	return NULL;
}


/*-----------------------------

 koImageFree()
-----------------------------*/
void koImageFree(struct ko_image* image)
{
	int i = 0;

	if (image->length < (uint32_t)UINT16_MAX)
	{
		for (i = 0; i < image->frames_no; i++)
		{
			if (image->data[i] != NULL)
				koFarFree(image->data[i]);
		}
	}

	koFree(image);
}
