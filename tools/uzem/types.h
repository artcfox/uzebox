/*
Basic type definitions

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

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef  int_fast32_t  asint;  /* Architecture signed integer */
typedef uint_fast32_t  auint;  /* Architecture unsigned integer */
typedef       int16_t  sint16;
typedef      uint16_t  uint16;
typedef       int32_t  sint32;
typedef      uint32_t  uint32;
typedef        int8_t  sint8;
typedef       uint8_t  uint8;

/* Defined maximum & minimums for auints and asints */
#define AUINT_MAX UINT_FAST32_MAX
#define AUINT_MIN UINT_FAST32_MIN
#define ASINT_MAX INT_FAST32_MAX
#define ASINT_MIN INT_FAST32_MIN

#endif
