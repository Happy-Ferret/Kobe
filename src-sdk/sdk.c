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

 [sdk.c]
 - Alexander Brandt 2018
-----------------------------*/

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kimage.h"
#include "kpalette.h"


static char version_text[] = ""
"Kobe SDK v0.2\n\n"
"2018 - Alexander Brandt.\n"
"Distributed under the 'MIT License'.\n\n";

static char help_text[] = ""
"Kobe SDK v0.2\n\n"
"Usage: sdk <mode> -i <filename> [options]\n"
"\n"
"  Where <mode> use: (*required)\n"
"    image\n"
"    palette\n"
"\n"
"General Options:\n"
"  -h, --help                    Prints this screen\n"
"  -v, --version                 Prints version\n"
"  -i, --input <filename>        Input filename (*required)\n"
"  -o, --output <filename>       Output filename\n"
"\n"
"Image options:\n"
"  -a, --arrange <arrange>       Image bytes order\n"
"  -f, --frame <dimensions>      Frames dimensions, width and height\n"
"                                separated with an 'x', ie: '32x64'\n"
"\n"
"  Where <arrange> use:\n"
"    planes                      Honor to VGA planes (default)\n"
"    columns                     Top to bottom\n"
"    linear                      Left to right - Top to bottom\n"
"\n";

enum mode
{
	UNDEFINED = 0,
	IMAGE,
	PALETTE
};

struct arguments
{
	enum mode mode;

	char* input_filename;
	char* output_filename;

	enum arrange arrange; // Image

	int frame_w; // Image
	int frame_h; // Image

	bool quit;
	int quit_value;
};


/*-----------------------------

 GetWidthXHeight()
-----------------------------*/
int GetWidthXHeight(const char* arg, int* width, int* height)
{
	char buffer[16];
	char* caret = buffer;

	bool width_done = false;

	for (const char* c = arg;; c++, caret++)
	{
		*caret = *c;

		if (caret > (buffer + 16))
		{
			fprintf(stderr, "[Error] Out of memory\n");
			return 1;
		}

		if (width_done == false)
		{
			if (*caret == '\0')
				goto incorrect_format;

			if (isdigit(*caret) == 0)
			{
				if (*caret != 'x')
					goto unexpected_character;

				*caret = '\0';

				if (strlen(buffer) == 0)
					goto incorrect_format;

				*width = atoi(buffer);
				width_done = true;
				caret = buffer - 1;
			}
		}
		else
		{
			if (isdigit(*caret) == 0)
			{
				if (*caret != '\0')
					goto unexpected_character;

				if (strlen(buffer) == 0)
					goto incorrect_format;

				*height = atoi(buffer);
				return 0;
			}
		}
	}

incorrect_format:
	fprintf(stderr, "[Error] Incorrect format in argument '%s'\n", arg);
	return 1;

unexpected_character:
	fprintf(stderr, "[Error] Unexpected character '%c' in argument '%s'\n", *caret, arg);
	return 1;
}


/*-----------------------------

 CheckArguments()
-----------------------------*/
static void CheckArguments(int argc, char* argv[], struct arguments* out)
{
	// Default values
	out->mode = UNDEFINED;
	out->input_filename = NULL;
	out->output_filename = NULL;
	out->arrange = PLANES;
	out->frame_w = 0;
	out->frame_h = 0;
	out->quit = false;
	out->quit_value = EXIT_SUCCESS;

	if (argc == 1)
	{
		printf("%s", help_text);
		out->quit = true;
		return;
	}

	// Arguments
	bool skip_next = false;

	for (int i = 1; i < argc; i++)
	{
		if (skip_next == true)
		{
			skip_next = false;
			continue;
		}

		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			printf("%s", help_text);
			out->quit = true;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
		{
			printf("%s", version_text);
			out->quit = true;
		}
		else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0)
		{
			if ((i + 1) < argc)
			{
				assert((out->input_filename = malloc(strlen(argv[i + 1]) + 1)) != NULL);
				strcpy(out->input_filename, argv[i + 1]);
				skip_next = true;
			}
		}
		else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
		{
			if ((i + 1) < argc)
			{
				assert((out->output_filename = malloc(strlen(argv[i + 1]) + 1)) != NULL);
				strcpy(out->output_filename, argv[i + 1]);
				skip_next = true;
			}
		}
		else if (strcmp(argv[i], "image") == 0)
		{
			if (out->mode == UNDEFINED)
				out->mode = IMAGE;
		}
		else if (strcmp(argv[i], "palette") == 0)
		{
			if (out->mode == UNDEFINED)
				out->mode = PALETTE;
		}
		else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--arrange") == 0)
		{
			if ((i + 1) < argc)
			{
				skip_next = true;

				if (strcmp(argv[i + 1], "planes") == 0)
					out->arrange = PLANES;
				else if (strcmp(argv[i + 1], "columns") == 0)
					out->arrange = COLUMNS;
				else if (strcmp(argv[i + 1], "linear") == 0)
					out->arrange = LINEAR;
				else
					fprintf(stderr, "[Warning] Unknown arrange '%s'\n", argv[i + 1]);
			}
		}
		else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--frame") == 0)
		{
			if ((i + 1) < argc)
			{
				skip_next = true;

				if (GetWidthXHeight(argv[i + 1], &out->frame_w, &out->frame_h) != 0)
				{
					out->quit = true;
					out->quit_value = EXIT_FAILURE;
				}
			}
		}
		else
		{
			fprintf(stderr, "[Warning] Unknown argument '%s'\n", argv[i]);
			continue;
		}
	}

	// Nothing to do here
	if (out->quit == true)
		return;

	// Mode specified?
	if (out->mode == UNDEFINED)
	{
		fprintf(stderr, "[Error] Mode not specified\n");
		out->quit = true;
		out->quit_value = EXIT_FAILURE;
		return;
	}

	// Input specified?
	if (out->input_filename == NULL || strlen(out->input_filename) == 0)
	{
		fprintf(stderr, "[Error] Input not specified\n");
		out->quit = true;
		out->quit_value = EXIT_FAILURE;
		return;
	}

	// Output specified?
	if (out->output_filename == NULL || strlen(out->output_filename) == 0)
	{
		if (out->output_filename != NULL)
			free(out->output_filename);

		assert((out->output_filename = malloc(strlen(out->input_filename) + strlen(".kx") + 2)) != NULL);
		strcpy(out->output_filename, out->input_filename);

		// Replace extension
		char* c = NULL;
		char* extension = ".png";
		char* replacement = ".ki";

		if (out->mode == PALETTE)
			replacement = ".kp";

		for (c = out->output_filename + strlen(out->output_filename); c != out->output_filename; c--)
		{
			if (*c == '/')
				goto append;

			if (strcmp(c, extension) == 0)
			{
				strcpy(c, replacement);
				break;
			}
		}

		// Append extension
	append:
		if (c == out->output_filename)
			strcat(out->output_filename, replacement);
	}
}


/*-----------------------------

 FreeArguments()
-----------------------------*/
static inline void FreeArguments(struct arguments* args)
{
	if (args->input_filename != NULL)
		free(args->input_filename);

	if (args->output_filename != NULL)
		free(args->output_filename);
}


/*-----------------------------

 main()
-----------------------------*/
int main(int argc, char* argv[])
{
	struct arguments args;
	int status = 0;

	CheckArguments(argc, argv, &args);

	if (args.quit == true)
	{
		FreeArguments(&args);
		return args.quit_value;
	}

	switch (args.mode)
	{
	case IMAGE:
		status = ConvertToKimage(args.input_filename, args.output_filename, args.arrange, args.frame_w, args.frame_h);
		break;
	case PALETTE:
		status = ConvertToKpalette(args.input_filename, args.output_filename);
		break;
	default:
		status = 1;
	}

	FreeArguments(&args);

	if (status != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
