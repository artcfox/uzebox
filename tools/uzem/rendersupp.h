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


#ifndef RENDERSUPP_H
#define RENDERSUPP_H

#include "types.h"
#include "SDL2/SDL.h"


// Renders a line into a 32 bit output buffer.
//
// This can be used to produce progressive scan output. Performs a shrink by
// 2,333333 as required for the aspect ratio of Uzebox. Can be used with any
// channel order as long as there are 8 bits per component and the palette is
// set up proper.
//
// dest0 is where the line will be output at full intensity
// dest1 is where it will be output with 3/4 (scanline effect)
// off is the offset in the source (input line buffer; width is 2048 elements)
// len is the count of pixels to render (truncated to largest multiple of 3)
//
void rendersupp_line32(uint32* dest0, uint32* dest1,
                       uint8 const* src, auint off, auint len, auint const* pal);


// Outputs into 0x00RRGGBB output from an SDL surface.
//
// lno is the line number to read from the surface
// off is the start pixel offset within the line
// len is the count of pixels to output
//
void rendersupp_convsurf(uint32* dest, SDL_Surface const* src,
                         auint lno, auint off, auint len);


#endif
