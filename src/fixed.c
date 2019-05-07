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

 [fixed.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "fixed.h"


/*-----------------------------

 koMultiplyFixed()
-----------------------------*/
ko_fixed_t koMultiplyFixed(ko_fixed_t a, ko_fixed_t b)
{
	/*
	 TODO: rather than SHR, MOV, SHL and OR... is possible to
 	 use an SHRD? like Abrash (see below)
	*/

	ko_fixed_t result = 0;

#if defined(__WATCOMC__) && _M_IX86 >= 300 /* Real mode i386 */

	_asm
	{
		PUSH eax		; TODO: see the call type
		PUSH edx

		MOV eax, a
		IMUL b			; Result in EDX:EAX

		SHR eax, 16		; Shift the fraction part (right), considering only the low 16bits
		MOV result, eax	; Copy to the result

		SHL edx, 16		; Shift the whole part (left)
						; Considering only the lower 16 bits of edx by moving them to
						; the high part of the register, this for the next OR operation

		OR result, edx	; 'Copy' to the result

		POP edx
		POP eax
	}

#else

	/* All the same as above, but in C */
	int64_t temp64 = 0;

	temp64 = (int64_t)a * (int64_t)b;
	result = (uint32_t)((uint64_t)temp64 & 0x00000000FFFFFFFF) >> 16; /* Fraction part*/
	result |= (int32_t)((int64_t)temp64 >> 32) << 16;                 /* Whole part */

#endif

	return result;
}


/*-----------------------------

 koDivideFixed()
-----------------------------*/
ko_fixed_t koDivideFixed(ko_fixed_t dividend, ko_fixed_t divisor)
{
	ko_fixed_t result = 0;

#if defined(__WATCOMC__) && _M_IX86 >= 300 /* Real mode i386 */

	_asm
	{
		PUSH edx
		PUSH eax

		; Purest witchcraft
		MOV edx, dividend
		SHRD eax, edx, 16
		SAR edx, 16

		IDIV divisor
		MOV result, eax

		POP edx
		POP eax
	}

#else

	/*
	 Not the same as above, but a bad simplification, if I not wrong
	 Abrash managed to omit a MOV using SHRD. Also in the original
	 code he do a somewhat sign test whit a SUB but whitout considering
	 the side effects (I think)... whatever, procure never divide anything.
	*/

	int32_t edx = dividend;  /* MOV edx, dividend */
	uint32_t eax = dividend; /* MOV eax, dividend */

	/* TODO: The difference betwen the next SAR and SHL, is implicit on
	 the C signed/unsigned types? */
	edx = edx >> 16; /* SAR edx, 16 */
	eax = eax << 16; /* SHL eax, 16 */

	/* With lucko, translated into a IDIV instruction: */
	result = ((int64_t)eax | (int64_t)edx << 32) / ((int64_t)divisor);

#endif

	return result;
}
