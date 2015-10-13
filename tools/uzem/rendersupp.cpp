/*
Renderer assistance functions

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


#include "rendersupp.h"



void rendersupp_line32(uint32* dest0, uint32* dest1,
                       uint8 const* src, auint off, auint len, auint const* pal)
{
	auint  sp = off;
	auint  dp;
	auint  px;
	auint  t0, t1, t2, t3, t4;

	// Note: This function relies on the destination using a 8 bits per
	// channel representation, but the channel order is irrelevant.

	t0 = (pal[src[(sp - 1U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
	t1 = (pal[src[(sp + 0U) & 0x7FFU]] & 0xF0F0F0F0U) >> 4;
	for (dp = 0U; dp < ((len / 3U) * 3U); dp += 3U)
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



// Returns the count of trailing zeroes
// For the explanation of the algorithm, see De Brujin sequences; this is a
// known bit hack to perform this operation, and it is the fastest for the
// purposes of interpreting the masks.
// (Note: SDL's pixelformat structure technically has this info, but reserved
// for internal use, so to be correct it can not be used)
static const auint rendersupp_debrujintb[32] = {
	 0U,  1U, 28U,  2U, 29U, 14U, 24U,  3U,
	30U, 22U, 20U, 15U, 25U, 17U,  4U,  8U,
	31U, 27U, 13U, 23U, 21U, 19U, 16U,  7U,
	26U, 12U, 18U,  6U, 11U,  5U, 10U,  9U
};
static auint rendersupp_getmaskpos(auint mask)
{
	return rendersupp_debrujintb[(((mask & (0U - mask)) * 0x077CB531U) >> 27) & 0x1FU];
}



void rendersupp_convsurf(uint32* dest, SDL_Surface const* src,
                         auint lno, auint off, auint len)
{
	if (src->format->BytesPerPixel == 4U)
	{

		uint32 const* sptr = (uint32 const*)(&(((uint8 const*)(src->pixels))[((src->pitch) * lno) + off]));

		if      ( ((src->format->Rmask) == 0x00FF0000U) &&
		          ((src->format->Gmask) == 0x0000FF00U) &&
		          ((src->format->Bmask) == 0x000000FFU) )
		{
			for (auint i = 0U; i < len; i++)
			{
				dest[i] = sptr[i] & 0x00FFFFFFU;
			}
		}
		else if ( ((src->format->Rmask) == 0x000000FFU) &&
		          ((src->format->Gmask) == 0x0000FF00U) &&
		          ((src->format->Bmask) == 0x00FF0000U) )
		{
			for (auint i = 0U; i < len; i++)
			{
				dest[i] = ((sptr[i] & 0x000000FFU) << 16) |
				          ((sptr[i] & 0x0000FF00U)      ) |
				          ((sptr[i] & 0x00FF0000U) >> 16);
			}
		}
		else
		{
			auint rsh = rendersupp_getmaskpos(src->format->Rmask);
			auint gsh = rendersupp_getmaskpos(src->format->Gmask);
			auint bsh = rendersupp_getmaskpos(src->format->Bmask);
			auint rms = (src->format->Rmask) >> rsh;
			auint gms = (src->format->Gmask) >> gsh;
			auint bms = (src->format->Bmask) >> bsh;
			while (rms > 0xFFU) // More than 8 bits per channel
			{
				rms >>= 1U;
				rsh  += 1U;
			}
			while (gms > 0xFFU)
			{
				gms >>= 1U;
				gsh  += 1U;
			}
			while (bms > 0xFFU)
			{
				bms >>= 1U;
				bsh  += 1U;
			}
			for (auint i = 0U; i < len; i++)
			{
				dest[i] = (((sptr[i] >> rsh) & 0xFFU) << 16) |
				          (((sptr[i] >> gsh) & 0xFFU) <<  8) |
				          (((sptr[i] >> bsh) & 0xFFU)      );
			}
		}

	}
}
