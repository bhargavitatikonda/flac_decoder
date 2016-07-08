/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name    : bitmath.h
 * $Rev: 934 $
 * $Date:: 2016-06-09 12:43:04 +0900#$
 */

/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2001-2009  Josh Coalson
 * Copyright (C) 2011-2014  Xiph.Org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAC__PRIVATE__BITMATH_H
#define FLAC__PRIVATE__BITMATH_H

#include "FLAC/ordinals.h"
#include "FLAC/assert.h"						/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00014 */

/* for CHAR_BIT */
#define CHAR_BIT      8         /* number of bits in a char */
#include "share/compat.h"

/* Will never be emitted for MSVC, GCC, Intel compilers */
static __inline unsigned int FLAC__clz_soft_uint32(unsigned int word)
{
    static const unsigned char byte_to_unary_table[] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    return (word) > 0xffffff ? byte_to_unary_table[(word) >> 24] :
    (word) > 0xffff ? byte_to_unary_table[(word) >> 16] + 8 :
    (word) > 0xff ? byte_to_unary_table[(word) >> 8] + 16 :
    byte_to_unary_table[(word)] + 24;
}

static __inline unsigned int FLAC__clz_uint32(FLAC__uint32 v)
{
    return FLAC__clz_soft_uint32(v);
}

/* This one works with input 0 */
static __inline unsigned int FLAC__clz2_uint32(FLAC__uint32 v)
{
    if (!v)
        return 32;
    return FLAC__clz_uint32(v);
}

/* An example of what FLAC__bitmath_ilog2() computes:
 *
 * ilog2( 0) = assertion failure
 * ilog2( 1) = 0
 * ilog2( 2) = 1
 * ilog2( 3) = 1
 * ilog2( 4) = 2
 * ilog2( 5) = 2
 * ilog2( 6) = 2
 * ilog2( 7) = 2
 * ilog2( 8) = 3
 * ilog2( 9) = 3
 * ilog2(10) = 3
 * ilog2(11) = 3
 * ilog2(12) = 3
 * ilog2(13) = 3
 * ilog2(14) = 3
 * ilog2(15) = 3
 * ilog2(16) = 4
 * ilog2(17) = 4
 * ilog2(18) = 4
 */

static __inline unsigned FLAC__bitmath_ilog2(FLAC__uint32 v)
{
    FLAC__ASSERT(v > 0);						/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00014 */
    return (FLAC__uint32)(sizeof(FLAC__uint32) * CHAR_BIT  - 1 - FLAC__clz_uint32(v));
}

#endif
