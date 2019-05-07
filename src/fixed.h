/*-----------------------------

 [fixed.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_FIXED_H
#define KO_FIXED_H

	#include "kobe.h"

	#define KO_DECIMAL_075 0xC000
	#define KO_DECIMAL_05 0x8000
	#define KO_DECIMAL_025 0x4000
	#define KO_DECIMAL_0125 0x2000

	typedef int32_t ko_fixed_t;

	#define koMakeFixed(whole, fraction) ((uint32_t)fraction) | ((int32_t)whole << 16)

	#if 0
	#define koFixedFromFloat(no) (int32_t)((float)no * 65536.0 + 0.5)
	#define koFixedToFloat(no) ((float)no) / 65536.0
	#endif

	#define koFixedWhole(f) (int16_t)((int32_t)f >> 16)
	#define koFixedFraction(f) (uint16_t)((uint32_t)f & 0x0000FFFF)

	ko_fixed_t koMultiplyFixed(ko_fixed_t a, ko_fixed_t b);
	ko_fixed_t koDivideFixed(ko_fixed_t a, ko_fixed_t b);

#endif
