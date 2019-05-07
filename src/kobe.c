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

 [kobe.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "kobe.h"
#include "util.h"

#include "input.h"
#include "render.h"
#include "time.h"

#include "image.h"
#include "palette.h"
#include "tests.h"


/*-----------------------------

 Globals
-----------------------------*/
const struct ko_input_state* g_input;
const struct ko_render_state* g_render;
uint8_t KO_HUGE* g_background_memory;

static struct ko_input_cfg s_input_cfg;


/*-----------------------------

 main()
-----------------------------*/
int main(int argc, char* argv[])
{
	struct ko_image* background = NULL;
	struct ko_image* sprite = NULL;
	uint32_t start = 0;
	uint32_t engine_time = 0;

	/* Tests */
	if (argc > 1)
	{
		if (strcmp(argv[1], "test1") == 0)
			return koBackgroundTest();
		else if (strcmp(argv[1], "test2") == 0)
			return koScrollTest();
		else if (strcmp(argv[1], "test3") == 0)
			return koSpriteTest();
	}

	koPrint(KO_LOG, "###\n\n%s v%u.%u\n\n", KO_NAME, KO_VMAX, KO_VMIN);

	/* Initialization */
	s_input_cfg.left_bind = 75;   /* 75 = left arrow */
	s_input_cfg.right_bind = 77;  /* 77 = right arrow */
	s_input_cfg.up_bind = 72;     /* 72 = up arrow */
	s_input_cfg.down_bind = 80;   /* 80 = down arrow */
	s_input_cfg.a_bind = 44;      /* 40 = 'z' key */
	s_input_cfg.b_bind = 45;      /* 40 = 'x' key */
	s_input_cfg.accept_bind = 28; /* 28 = Enter key */
	s_input_cfg.cancel_bind = 1;  /* 1 = Escape key */

	if (koInputInit(&s_input_cfg) != 0 || koInitTimeModule() != 0 ||
	    koRenderInit("res/palette.kp", KO_LOGICAL_WIDTH) != 0 ||
	    (g_background_memory = koHugeAlloc(KO_BACKGROUND_MEMORY_LEN)) == NULL)
		goto return_failure;

	/* Resources */
	if ((background = koImageLoad("res/bkg01.ki", g_background_memory)) == NULL
	|| (sprite = koImageLoad("res/bcrate02.ki", NULL)) == NULL)
		goto return_failure;

	/* Game loop */
	koRenderOffset(320);
	koRenderBackground(background);

	koRenderSpriteCreate(sprite, 480, 100);

	koPrint(KO_LOG, "\n");

	while (1)
	{
		start = koGetTime();

		/* Render previous frame, get current input state */
		if ((g_render = koRenderUpdate()) == NULL || (g_input = koInputUpdate()) == NULL)
			goto return_failure;

		/* Game logic */
		if (g_input->right == true)
			koRenderOffset(g_input->a ? g_render->offset + 8 : g_render->offset + 4);

		if (g_input->left == true)
			koRenderOffset(g_input->a ? g_render->offset - 8 : g_render->offset - 4);

		if (g_input->cancel == true)
		{
			koPrint(KO_LOG, "Break signal\n\n");
			break;
		}

		engine_time = koGetTime() - start;

		/* Zzz */
		if (engine_time <= 33)
			koSleep(33 - engine_time); /* ~30hz */
	}

	/* Bye! */
	koImageFree(background);
	koHugeFree(g_background_memory);

	koStopTimeModule();
	koInputStop();
	koRenderStop();
	return 0;

return_failure:
	if (background != NULL)
		koImageFree(background);

	if (g_background_memory != NULL)
		koHugeFree(g_background_memory);

	koStopTimeModule();
	koInputStop();
	koRenderStop();
	return 1;
}
