/*-----------------------------

 [kimage.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KIMAGE_H
#define KIMAGE_H

	enum arrange
	{
		LINEAR = 0x00,
		COLUMNS = 0x01,
		PLANES = 0x02,
		UNUSED = 0x03
	};

	int ConvertToKimage(const char* input_filename, const char* output_filename, enum arrange arrange, int frame_w, int frame_h);

#endif
