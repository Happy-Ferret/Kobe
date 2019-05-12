/*-----------------------------

 [kobe.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_KOBE_H
#define KO_KOBE_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>
	#include <time.h>

	#define KO_NAME "Kobe"
	#define KO_VMAX 0
	#define KO_VMIN 1

	#define KO_BACKGROUND_MEMORY_LEN 192000
	#define KO_LOGICAL_WIDTH 960
	#define KO_LOGICAL_HEIGHT 240

	#define KO_FILENAME_LENGTH 12

	/* Watcom compiler, Intel x86 (real mode) */
	#ifdef __WATCOMC__
	#ifdef __I86__

		#include <i86.h>
		#include <dos.h>
		#include <conio.h>
		#include <malloc.h>

		/* C99 types */
		typedef signed char int8_t;
		typedef signed short int16_t;
		typedef signed long int32_t;
		typedef signed long long int64_t;

		typedef unsigned char uint8_t;
		typedef unsigned short uint16_t;
		typedef unsigned long uint32_t;
		typedef unsigned long long uint64_t;

		#define UINT8_MAX 255
		#define UINT16_MAX 65535
		#define UINT32_MAX 4294967295

		#define true 1
		#define false 0
		typedef char bool;

		/* i86.h */
		#define KO_HUGE __huge
		#define KO_FAR __far
		#define KO_NEAR __near
		#define KO_INTERRUPT __interrupt

		#define ko_regs REGS
		#define ko_status_regs SREGS

		#define koDosPointerOffset(p) FP_OFF(p)
		#define koDosPointerSegment(p) FP_SEG(p)
		#define koDosPointerMakeFar(s, o) MK_FP(s, o)

		#define koDosInterrupt(i, ri, ro) int86(i, ri, ro);
		#define koDosInterruptX(i, r1, ro, sr) int86x(i, r1, ro, sr);

		/* dos.h */
		#define koDosVectorGet(n) _dos_getvect(n)
		#define koDosVectorSet(n, v) _dos_setvect(n, v)

		/* conio.h */
		#define koDosPortOut(p, v) outp(p, v)
		#define koDosPortIn(p) inp(p)

		/* malloc.h */
		#define koDosHeapGrow() _nheapgrow()

		#define koDosFarMemoryAlloc(s) _fmalloc(s)
		#define koDosFarMemoryFree(p) _ffree(p)

		#define koDosHugeMemoryAlloc(n, s) halloc(n, s)
		#define koDosHugeMemoryFree(p) hfree(p)

		#define koDosFarMemorySet(p, v, s) _fmemset(p, v, s)
		#define koDosFarMemoryCopy(d, o, s) _fmemcpy(d, o, s)

	#endif
	#endif

	/* Clang/LLVM Compiler, for diagnostic purposes only */
	#if defined(__llvm__)

		#include <stdint.h>
		#include <stdbool.h>

		#define KO_HUGE /* whitespace */
		#define KO_FAR /* whitespace */
		#define KO_NEAR /* whitespace */
		#define KO_INTERRUPT /* whitespace */

		union ko_regs
		{
			struct
			{
				uint16_t ax, bx, cx, dx, si, di;
				uint32_t cflag; /* TODO: use 16 bits? */
			} w;

			struct
			{
				uint8_t al, ah, bl, bh;
				uint8_t cl, ch, dl, dh;
			} h;

		};

		struct ko_status_regs
		{
			uint16_t es, cs, ss, ds;
		};

		extern uint16_t koDosPointerOffset(const void*);
		extern uint16_t koDosPointerSegment(const void*);
		extern uint32_t koDosPointerMakeFar(uint16_t segment, uint16_t offset);

		extern int16_t koDosInterrupt(int, union ko_regs*, union ko_regs*);
		extern int16_t koDosInterruptX(int, union ko_regs*, union ko_regs*, struct ko_status_regs*);

		extern void koDosPortOut(uint16_t pointer, uint8_t vector);
		extern uint8_t koDosPortIn(uint16_t pointer);

		extern void KO_FAR* koDosVectorGet(uint8_t no);
		extern void koDosVectorSet(uint8_t no, void KO_INTERRUPT* vector);

		extern void KO_FAR* koDosFarMemoryAlloc(uint16_t size);
		extern void koDosFarMemoryFree(void KO_FAR* pointer);

		extern void KO_HUGE* koDosHugeMemoryAlloc(int32_t no, uint16_t size);
		extern void koDosHugeMemoryFree(void KO_HUGE* pointer);

		extern void koDosHeapGrow();

		extern void koDosFarMemorySet(void KO_FAR* pointer, int value, size_t size);
		extern void koDosFarMemoryCopy(void KO_FAR* destination, const void KO_FAR* origin, size_t size);

	#endif
#endif
