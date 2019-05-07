/*-----------------------------

 [image.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_IMAGE_H
#define KO_IMAGE_H

	#include "kobe.h"

	struct ko_image
	{
		int width;
		int height;
		uint32_t length;

		int frames_no;
		uint8_t KO_FAR** data;

		uint32_t KO_HUGE* data_huge;
	};

	struct ko_image* koImageLoad(const char* filename, uint8_t KO_HUGE* huge_destination);
	void koImageFree(struct ko_image* image);

#endif
