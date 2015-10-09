/*
Software renderer

-----

(The MIT License)

Copyright (c) 2008-2015 by
David Etherton, Eric Anderton, Alec Bourque (Uze), Filipe Rinaldi,
Sandor Zsuga (Jubatian), Matt Pandina (Artcfox)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "rendersoft.h"
#include <stdlib.h>



// Width of the display surface
#define DISPLAY_WIDTH   618U
// Height of the display surface
#define DISPLAY_HEIGHT  448U



renderSoft::renderSoft()
{
	o_status[0] = 0;
	o_hoff = RENDERIF_HOFF - (((DISPLAY_WIDTH / 3) * 7) / 2);
	o_voff = RENDERIF_VOFF - (DISPLAY_HEIGHT / 2);
	o_full = false;
	o_ssnum = 0U;
	o_isinit = false;
}



renderSoft::~renderSoft()
{
	destroy();
	// This is only for debugging. For some reason fullscreen now
	// doesn't work (jumbled up screen, although program runs), and
	// neither restores original resolution after return, even though
	// the destructor is called.
	printf("Renderer destroyed\n");
}



void renderSoft::destroy()
{
	if (!o_isinit)
	{
		return;
	}
	SDL_DestroyWindow(o_window);
	o_isinit = false;
}



bool renderSoft::init()
{
	destroy();

	o_window = SDL_CreateWindow(o_status,
	                            SDL_WINDOWPOS_CENTERED,
	                            SDL_WINDOWPOS_CENTERED,
	                            DISPLAY_WIDTH,
	                            DISPLAY_HEIGHT,
	                            o_full?SDL_WINDOW_FULLSCREEN:0U);
	if (o_window == NULL)
	{
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}

	o_wsurf = SDL_GetWindowSurface(o_window);
	if (o_wsurf == NULL)
	{
		SDL_DestroyWindow(o_window);
		fprintf(stderr, "SDL_GetWindowSurface failed: %s\n", SDL_GetError());
		return false;
	}

	if ((o_wsurf->format->BytesPerPixel) != 4U)
	{
		SDL_DestroyWindow(o_window);
		fprintf(stderr, "Display: Only 32 bit depth is supported (for now)\n");
		return false;
	}

	if (o_full)
	{
		SDL_ShowCursor(0);
	}

	for (auint i = 0U; i < 256U; i ++)
	{
		auint r = (((i >> 0) & 7U) * 0xFFU) / 7U;
		auint g = (((i >> 3) & 7U) * 0xFFU) / 7U;
		auint b = (((i >> 6) & 3U) * 0xFFU) / 3U;
		o_palette[i] = SDL_MapRGB(o_wsurf->format, r, g, b);
	}

	for (auint i = 0U; i < DISPLAY_HEIGHT; i ++)
	{
		o_isfill[i] = false;
	}

	o_prevl = 0U;

	o_isinit = true;
	return true;
}



// Renders a line into a 32 bit output buffer.
// Performs a shrink by 2,33
// dest0 is where the line will be output at full intensity
// dest1 is where it will be output with 3/4 (scanline effect)
// off is the offset in the source (source width is 2048 elements)
static void render_line(uint32* dest0, uint32* dest1,
                        uint8 const* src, auint off,
                        auint const* pal)
{
	auint  sp = off;
	auint  dp;
	auint  px;
	auint  t0, t1, t2, t3, t4;

	// Note: This function relies on the destination using a 8 bits per
	// channel representation, but the channel order is irrelevant.

	t0 = (pal[src[(sp - 1U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
	t1 = (pal[src[(sp + 0U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
	for (dp = 0U; dp < ((DISPLAY_WIDTH / 3U) * 3U); dp += 3U)
	{
		// Shrink roughly does this:
		// Source:      |----|----|----|----|----|----|----| (7px)
		// Destination: |-----------|----------|-----------| (3px)
		// Note that when combining pixels back together, in total a
		// multiplication with 17 will restore full intensity
		// (0xF * 17 = 0xFF)
		t2 = (pal[src[(sp + 1U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		t3 = (pal[src[(sp + 2U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		t4 = (pal[src[(sp + 3U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		px = (t0 * 4U) + (t1 * 4U) + (t2 * 4U) + (t3 * 4U) + (t4 * 1U);
		dest0[dp + 0U] = px;
		dest1[dp + 0U] = ((px & 0xFCFCFCFCU) >> 2) * 3U;
		t0 = (pal[src[(sp + 4U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		t1 = (pal[src[(sp + 5U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		px = (t2 * 2U) + (t3 * 4U) + (t4 * 5U) + (t0 * 4U) + (t1 * 2U);
		dest0[dp + 1U] = px;
		dest1[dp + 1U] = ((px & 0xFCFCFCFCU) >> 2) * 3U;
		t2 = (pal[src[(sp + 6U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		t3 = (pal[src[(sp + 7U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
		px = (t4 * 1U) + (t0 * 4U) + (t1 * 4U) + (t2 * 4U) + (t3 * 4U);
		dest0[dp + 2U] = px;
		dest1[dp + 2U] = ((px & 0xFCFCFCFCU) >> 2) * 3U;
		sp += 7U;
		t0 = t2;
		t1 = t3;
	}
}



void renderSoft::line(uint8 const* lbuf, auint llen, auint lno,
                      auint hsync, auint vsync)
{
	if (!o_isinit)
	{
		return;
	}

	// For now only support progressive scan

	if (o_prevl > lno) // New frame started
	{
		for (auint i = 0U; i < DISPLAY_HEIGHT; i ++)
		{
			if (!o_isfill[i])
			{
				memset((uint8*)(o_wsurf->pixels) + (i * o_wsurf->pitch), 0U, DISPLAY_WIDTH * 4U);
			}
			o_isfill[i] = false;
		}
		SDL_UpdateWindowSurface(o_window);
	}

	// Note: For small line numbers, due to unsigned arithmetic, it wraps,
	// so the '<' check will fail proper.

	if ((lno - o_voff) < DISPLAY_HEIGHT)
	{
		auint voff = (lno - o_voff);
		render_line(
			(uint32*)((uint8*)(o_wsurf->pixels) + ((voff     ) * o_wsurf->pitch)),
			(uint32*)((uint8*)(o_wsurf->pixels) + ((voff ^ 1U) * o_wsurf->pitch)),
			lbuf, o_hoff,
			o_palette);
		o_isfill[voff     ] = true;
		o_isfill[voff ^ 1U] = true;
	}

	o_prevl = lno;
}



bool renderSoft::setProp(auint prop, asint val)
{
	return setProp(prop, val, false);
}


bool renderSoft::setProp(auint prop, asint val, bool delay)
{
	bool need_init = false;

	switch (prop)
	{
	case RENDERIF_PROP_HOFF:
		val += RENDERIF_HOFF - (((DISPLAY_WIDTH / 3) * 7) / 2);
		if      (val < 0)
		{
			o_hoff = 0U;
		}
		else if (val > 2048 - ((DISPLAY_WIDTH / 3) * 7))
		{
			o_hoff = 2048 - ((DISPLAY_WIDTH / 3) * 7);
		}
		else
		{
			o_hoff = (auint)(val);
		}
		break;

	case RENDERIF_PROP_VOFF:
		val += RENDERIF_VOFF - (DISPLAY_HEIGHT / 2);
		if      (val < 0)
		{
			o_voff = 0U;
		}
		else if (val > 525 - DISPLAY_HEIGHT)
		{
			o_voff = 525 - DISPLAY_HEIGHT;
		}
		else
		{
			o_voff = (auint)(val);
		}
		break;

	case RENDERIF_PROP_FULL:
		if (o_full && (val == 0))
		{
			o_full = false;
			need_init = true;
		}
		if (!o_full && (val != 0))
		{
			o_full = true;
			need_init = true;
		}
		break;

	default:
		break;
	}

	if (need_init && !delay && o_isinit)
	{
		return init();
	}
	else
	{
		return true;
	}
}



void renderSoft::tick()
{
	if (!o_isinit)
	{
		return;
	}
}



void renderSoft::setStatusStr(char const* str)
{
	auint i;

	for (i = 0U; i < (RENDERSOFT_STATUS_LEN - 1U); i++)
	{
		if (str[i] == 0)
		{
			break;
		}
		o_status[i] = str[i];
	}
	o_status[i] = 0;

	if (o_isinit)
	{
		SDL_SetWindowTitle(o_window, str);
	}
}



void renderSoft::screenShot()
{
	char ssbuf[64];
	sprintf(ssbuf, "uzem_%03u.bmp", (unsigned int)(o_ssnum++));
	printf("Saving screenshot to '%s'...\n", ssbuf);
	SDL_SaveBMP(o_wsurf, ssbuf);
}
