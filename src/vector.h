/*-----------------------------

 [vector.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_VEC2_H
#define KO_VEC2_H

	#include "kobe.h"
	#include "fixed.h"

	struct ko_vector
	{
		ko_fixed_t x;
		ko_fixed_t y;
	};

	#define koMakeVector(out, x, y)\
		{\
			out->x = x;\
			out->y = y;\
		}

	#define koCopyVector(out, v)\
		{\
			out->x = v->x;\
			out->y = v->y;\
		}

	#define kdAddVector(out, a, b)\
		{\
			out->x = a->x + b->x;\
			out->y = a->y + b->y;\
		}

	#define koSubtractVector(out, a, b)\
		{\
			out->x = a->x - b->x;\
			out->y = a->y - b->y;\
		}

	#define koMultiplyVector(out, a, b)\
		{\
			out->x = koMultiplyFixed(a->x, b->x);\
			out->y = koMultiplyFixed(a->y, b->y);\
		}

	#define koDivideVector(out, a, b)\
		{\
			out->x = koDivideFixed(a->x, b->x);\
			out->y = koDivideFixed(a->y, b->y);\
		}

	#define koFlipVector(out, v)\
		{\
			out->x = -v->x;\
			out->y = -v->y;\
		}

	#define koScaleVector(out, v, scale)\
		{\
			out->x = koMultiplyFixed(v->x, scale);\
			out->y = koMultiplyFixed(v->y, scale);\
		}

	#define koVectorDot(a, b) (koMultiplyFixed(a->x, b->x) + koMultiplyFixed(a->y, b->y))

#endif
