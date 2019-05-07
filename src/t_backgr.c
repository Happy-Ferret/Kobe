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

 [t_backgr.c]
 - Alexander Brandt 2018
-----------------------------*/

#include "kobe.h"
#include "util.h"

#include "image.h"
#include "render.h"
#include "time.h"


/*-----------------------------

 koBackgroundTest()
-----------------------------*/
int koBackgroundTest()
{
	uint32_t start = 0;
	uint32_t far_time = 0;
	uint32_t huge_time = 0;

	struct ko_image* far_back = NULL;
	struct ko_image* huge_back = NULL;
	uint8_t KO_HUGE* huge_memory = NULL;

	koPrint(KO_LOG, "###\n\nKobe Background Test\n\n");

	if (koInitTimeModule() != 0)
		goto return_failure;

	if (koRenderInit("res/palette.kp", 320) != 0)
		goto return_failure;

	if ((huge_memory = koHugeAlloc((uint32_t)960 * (uint32_t)200)) == NULL)
		goto return_failure;

	if ((far_back = koImageLoad("res/t_bkgfar.ki", NULL)) == NULL ||
	    (huge_back = koImageLoad("res/t_bkghug.ki", huge_memory)) == NULL)
		goto return_failure;

	/* Far background */
	start = koGetTime();
	koRenderBackground(far_back);

	if (koRenderUpdate() == NULL)
		goto return_failure;

	far_time = koGetTime() - start;

	koSleep(1000);
	koRenderClean(0);

	/* Huge background */
	start = koGetTime();
	koRenderBackground(huge_back);

	if (koRenderUpdate() == NULL)
		goto return_failure;

	huge_time = koGetTime() - start;

	koSleep(1000);

	/* Bye! */
	koImageFree(far_back);
	koImageFree(huge_back);
	koHugeFree(huge_memory);

	koStopTimeModule();
	koRenderStop();

	printf("Far background: %lu ms\n", far_time);
	printf("Huge background: %lu ms\n", huge_time);

	koPrint(KO_LOG, "Far background: %lu ms\n", far_time);
	koPrint(KO_LOG, "Huge background: %lu ms\n", huge_time);

	return 0;

return_failure:
	koPrint(KO_LOG, "[Error] BackgroundTest()\n");

	if (far_back != NULL)
		koImageFree(far_back);

	if (huge_back != NULL)
		koImageFree(huge_back);

	if (huge_memory != NULL)
		koHugeFree(huge_memory);

	koStopTimeModule();
	koRenderStop();

	return 1;
}
