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

 [t_sprite.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "kobe.h"
#include "util.h"

#include "image.h"
#include "render.h"
#include "time.h"


/*-----------------------------

 koSpriteTest()
-----------------------------*/
int koSpriteTest()
{
	uint32_t start = 0;
	uint32_t render_time = 0;
	const struct ko_render_state* render_state;

	koPrint(KO_LOG, "###\n\nKobe Sprite Test\n\n");

	if (koInitTimeModule() != 0)
		goto return_failure;

	if (koRenderInit("res/palette.kp", 960) != 0)
		goto return_failure;

	/* Test */
	start = koGetTime();

	if ((render_state = koRenderUpdate()) == NULL)
		goto return_failure;

	render_time = koGetTime() - start;
	koSleep(1000);

	/* Bye! */
	koStopTimeModule();
	koRenderStop();

	printf("Render time: %lu ms\n", render_time);
	koPrint(KO_LOG, "Render time: %lu ms\n", render_time);

	return 0;

return_failure:
	koPrint(KO_LOG, "[Error] SpriteTest()\n");

	koStopTimeModule();
	koRenderStop();

	return 1;
}
