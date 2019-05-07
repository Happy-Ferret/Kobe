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

 [input.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "input.h"

static void KO_FAR* s_key_old_vector;
static struct ko_input_cfg s_config;

static struct ko_input_state s_rt_state;
static struct ko_input_state s_state;


/*-----------------------------

 s_keyboard_vector()
-----------------------------*/
static void KO_INTERRUPT s_keyboard_vector()
{
	/*
	 www8.cs.umu.se/~isak/snippets/keywatch.c
	 https://stackoverflow.com/a/40963633
	 http://www.ctyme.com/intr/rb-0045.htm#Table6
	*/

	uint8_t raw = 0;

	raw = koDosPortIn(0x60);
	s_rt_state.last_key = raw;

	/* Key press */
	if (raw == s_config.left_bind)
		s_rt_state.left = true;
	else if (raw == s_config.right_bind)
		s_rt_state.right = true;
	else if (raw == s_config.up_bind)
		s_rt_state.up = true;
	else if (raw == s_config.down_bind)
		s_rt_state.down = true;
	else if (raw == s_config.a_bind)
		s_rt_state.a = true;
	else if (raw == s_config.b_bind)
		s_rt_state.b = true;
	else if (raw == s_config.accept_bind)
		s_rt_state.accept = true;
	else if (raw == s_config.cancel_bind)
		s_rt_state.cancel = true;

	/* Key release */
	else if (raw == s_config.left_bind + 128)
		s_rt_state.left = false;
	else if (raw == s_config.right_bind + 128)
		s_rt_state.right = false;
	else if (raw == s_config.up_bind + 128)
		s_rt_state.up = false;
	else if (raw == s_config.down_bind + 128)
		s_rt_state.down = false;
	else if (raw == s_config.a_bind + 128)
		s_rt_state.a = false;
	else if (raw == s_config.b_bind + 128)
		s_rt_state.b = false;
	else if (raw == s_config.accept_bind + 128)
		s_rt_state.accept = false;
	else if (raw == s_config.cancel_bind + 128)
		s_rt_state.cancel = false;

	koDosPortOut(0x20, 0x20); /* EOI to finish interrupt */
}


/*-----------------------------

 koInputInit()
-----------------------------*/
unsigned koInputInit(const struct ko_input_cfg* config)
{
	/*
	 http://retired.beyondlogic.org/interrupts/interupt.htm#2
	 http://www.ctyme.com/intr/rb-5956.htm
	*/
	// union ko_regs reg;

	koPrint(KO_LOG, "initializing input module...\n");

	if (config != NULL)
		memcpy(&s_config, config, sizeof(struct ko_input_cfg));

	/* Keyboard interruptor */
	s_key_old_vector = koDosVectorGet(0x09);
	koDosVectorSet(0x09, (void KO_INTERRUPT*)s_keyboard_vector);

	koPrint(KO_LOG, " - keyboard old vector: %04X:%04X\n", koDosPointerSegment(s_key_old_vector),
	             koDosPointerOffset(s_key_old_vector));
	koPrint(KO_LOG, " - keyboard new vector: %04X:%04X\n", koDosPointerSegment((void*)&s_keyboard_vector),
	             koDosPointerOffset((void*)&s_keyboard_vector));

	/* Mouse initialization */
	#if 0
	reg.w.ax = 0x0000;
	koDosInterrupt(0x33, &reg, &reg);

	if (reg.w.ax == 0xFFFF)
	{
		koPrint(KO_LOG, " - mouse driver found: 'Microsoft Mouse'\n");
		koPrint(KO_LOG, " - mouse low frequency: %s\n", s_config.mouse_low_frequency ? "true" : "false");
		s_rt_state.mouse_found = true;
	}
	#endif

	return 0; /* TODO: return errors and the presence of a mouse */
}


/*-----------------------------

 koInputUpdate()
-----------------------------*/
const struct ko_input_state* koInputUpdate()
{
	/*
	 http://www.ctyme.com/intr/rb-5967.htm
	*/
	// union ko_regs reg;

	#if 0
	/* Call mouse driver */
	if ((s_rt_state.mouse_found = true) && ((s_rt_state.mouse_tick += s_config.mouse_low_frequency) % 2) == 0)
	{
		s_rt_state.old_mouse_x = s_rt_state.mouse_x;
		s_rt_state.old_mouse_y = s_rt_state.mouse_y;

		reg.w.ax = 0x000B;
		koDosInterrupt(0x33, &reg, &reg);

		s_rt_state.mouse_x += reg.w.cx;
		s_rt_state.mouse_y += reg.w.dx;

		if (s_rt_state.mouse_x < 0)
			s_rt_state.mouse_x = 0;
		else if (s_rt_state.mouse_x > s_config.mouse_max_x - 1)
			s_rt_state.mouse_x = s_config.mouse_max_x - 1;

		if (s_rt_state.mouse_y < 0)
			s_rt_state.mouse_y = 0;
		else if (s_rt_state.mouse_y > s_config.mouse_max_y)
			s_rt_state.mouse_y = s_config.mouse_max_y;
	}
	#endif

	/* Our state */
	memcpy(&s_state, &s_rt_state, sizeof(struct ko_input_state));
	return &s_state;
}


/*-----------------------------

 koInputWait()
-----------------------------*/
void koInputWait(uint8_t key)
{
	while (1)
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

		if (s_rt_state.last_key == key)
		{
			s_rt_state.last_key = 0x00;
			break;
		}
	}
}


/*-----------------------------

 koInputStop()
-----------------------------*/
void koInputStop()
{
	koPrint(KO_LOG, "stopping input module...\n");

	if (s_key_old_vector != 0)
	{
		koDosVectorSet(0x09, s_key_old_vector);
		koPrint(KO_LOG, " - restored keyboard vector: %04X:%04X\n", koDosPointerSegment(s_key_old_vector),
		             koDosPointerOffset(s_key_old_vector));
	}

	memset(&s_rt_state, 0, sizeof(struct ko_input_state));
	memset(&s_state, 0, sizeof(struct ko_input_state));
}
