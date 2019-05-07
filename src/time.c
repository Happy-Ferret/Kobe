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

 [time.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "time.h"
#include "util.h"

#define CLOCK_MILI_DIVISOR 1193 /* PIT frequency/1000 */

static uint32_t s_clock_miliseconds = 0;
static void KO_FAR* s_clock_old_vector;


/*-----------------------------

 s_clock_vector()
-----------------------------*/
static void KO_INTERRUPT s_clock_vector()
{
	s_clock_miliseconds += 1;
	koDosPortOut(0x20, 0x20); /* To finish interrupt */
}


/*-----------------------------

 koInitTimeModule()
-----------------------------*/
unsigned koInitTimeModule()
{
	/*
	 http://retired.beyondlogic.org/interrupts/interupt.htm#2
	 http://www.osdever.net/bkerndev/Docs/pit.htm
	 http://www.intel-assembler.it/portale/5/Programming-the-Intel-8253-8354-pit/howto-program-intel-pit-8253-8254.asp
	*/

	koPrint(KO_LOG, "Initializing time module...\n");

	/* Clock interruptor */
	s_clock_old_vector = koDosVectorGet(0x08);
	koDosVectorSet(0x08, (void KO_INTERRUPT*)&s_clock_vector);

	koPrint(KO_LOG, " - old vector: %04X:%04X\n", koDosPointerSegment(s_clock_old_vector),
	        koDosPointerOffset(s_clock_old_vector));
	koPrint(KO_LOG, " - new vector: %04X:%04X\n", koDosPointerSegment((void*)&s_clock_vector),
	        koDosPointerOffset((void*)&s_clock_vector));

	/* Set frequency */
	koDosPortOut(0x43, 0x36);                      /* Command byte 0x36 */
	koDosPortOut(0x40, CLOCK_MILI_DIVISOR & 0xFF); /* Low byte of divisor */
	koDosPortOut(0x40, CLOCK_MILI_DIVISOR >> 8);   /* High byte of divisor */

	return 0; /* TODO: check for errors! */
}


/*-----------------------------

 koStopTimeModule()
-----------------------------*/
void koStopTimeModule()
{
	koPrint(KO_LOG, "Stopping time module...\n");

	if (s_clock_old_vector != 0)
	{
		koPrint(KO_LOG, " - at time: %lu sec\n", koGetTime() / 1000);
		koDosVectorSet(0x08, s_clock_old_vector);

		/* Restore frequency (18.2) */
		koDosPortOut(0x43, 0x36);
		koDosPortOut(0x40, 0);
		koDosPortOut(0x40, 0);

		koPrint(KO_LOG, " - restored vector: %04X:%04X\n", koDosPointerSegment(s_clock_old_vector),
		        koDosPointerOffset(s_clock_old_vector));
	}
}


/*-----------------------------

 koGetTime()
-----------------------------*/
uint32_t koGetTime() { return s_clock_miliseconds; }


/*-----------------------------

 koSleep()
-----------------------------*/
void koSleep(uint32_t ms)
{
	ms += s_clock_miliseconds;

	while (s_clock_miliseconds < ms)
	{
#ifdef __WATCOMC__
		_asm
		{
			NOP
			NOP
			NOP
			NOP
			HLT
		}
#endif
	}
}
