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

 [t_scroll.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "kobe.h"
#include "util.h"

#include "image.h"
#include "render.h"
#include "time.h"


/*-----------------------------

 koScrollTest()
-----------------------------*/
int koScrollTest()
{
	uint32_t start = 0;
	uint32_t render_time = 0;
	uint32_t avg_time = 0;
	const struct ko_render_state* render_state;

	struct ko_image* background = NULL;
	uint8_t KO_HUGE* huge_memory = NULL;

	koPrint(KO_LOG, "###\n\nKobe Scroll Test\n\n");

	if (koInitTimeModule() != 0)
		goto return_failure;

	if (koRenderInit("res/palette.kp", 960) != 0)
		goto return_failure;

	if ((huge_memory = koHugeAlloc((uint32_t)960 * (uint32_t)200)) == NULL)
		goto return_failure;

	if ((background = koImageLoad("res/t_bkghug.ki", huge_memory)) == NULL)
		goto return_failure;

	/* Game loop */
	koRenderBackground(background);

	while (1)
	{
		start = koGetTime();

		if ((render_state = koRenderUpdate()) == NULL)
			goto return_failure;

		render_time = koGetTime() - start;

		koRenderOffset(render_state->offset + 4);

		/* Wait next frame */
		if (render_time <= 33)
			koSleep(33 - render_time); /* ~30hz */

		if (avg_time != 0)
			avg_time = (avg_time + render_time) / 2;
		else
			avg_time = render_time;

		if (render_state->offset == (960 - 320))
			break;
	}

	koSleep(1000);

	/* Bye! */
	koImageFree(background);
	koHugeFree(huge_memory);

	koStopTimeModule();
	koRenderStop();

	printf("Average time: %lu ms\n", avg_time);
	koPrint(KO_LOG, "Average time: %lu ms\n", avg_time);

	return 0;

return_failure:
	koPrint(KO_LOG, "[Error] ScrollTest()\n");

	if (background != NULL)
		koImageFree(background);

	if (huge_memory != NULL)
		koHugeFree(huge_memory);

	koStopTimeModule();
	koRenderStop();

	return 1;
}
