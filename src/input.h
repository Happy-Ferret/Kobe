/*-----------------------------

 [input.h]
 - Alexander Brandt 2018
-----------------------------*/

#ifndef KO_INPUT_H
#define KO_INPUT_H

	#include "kobe.h"
	#include "util.h"

	struct ko_input_cfg
	{
		uint8_t left_bind;
		uint8_t right_bind;
		uint8_t up_bind;
		uint8_t down_bind;
		uint8_t a_bind;
		uint8_t b_bind;
		uint8_t accept_bind;
		uint8_t cancel_bind;

		bool mouse_low_frequency;
		int mouse_max_x;
		int mouse_max_y;
	};

	struct ko_input_state
	{
		/* Keyboard state */
		bool left;
		bool right;
		bool up;
		bool down;
		bool a;
		bool b;
		bool accept;
		bool cancel;

		uint8_t last_key;

		/* Mouse state */
		bool mouse_found;

		bool mouse_click;
		int mouse_x;
		int mouse_y;

		int old_mouse_x;
		int old_mouse_y;

		unsigned mouse_tick;
	};

	unsigned koInputInit(const struct ko_input_cfg*);
	void koInputStop();

	const struct ko_input_state* koInputUpdate();
	void koInputWait(uint8_t key);

#endif
