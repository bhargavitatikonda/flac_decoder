/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name   : compat.h
 * $Rev: 934 $
 * $Date:: 2016-06-09 12:43:04 +0900#$
 */

/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2012-2014  Xiph.org Foundation
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

/* This is the prefered location of all CPP hackery to make $random_compiler
 * work like something approaching a C99 (or maybe more accurately GNU99)
 * compiler.
 *
 * It is assumed that this header will be included after "config.h".
 */

#ifndef FLAC__SHARE__COMPAT_H
#define FLAC__SHARE__COMPAT_H

/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00010 start */
#if defined __INTEL_COMPILER || (defined _MSC_VER && defined _WIN64)
/* MSVS generates VERY slow 32-bit code with __restrict */
#define flac_restrict __restrict
#elif defined __GNUC__
#define flac_restrict __restrict__
#else
#define flac_restrict
#endif
/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00010 end */

/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00011 start */
#define FLAC__U64L(x) x##ULL
/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00011 end */

/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00012 start */
#ifndef M_LN2
	#define M_LN2 0.69314718055994530942 
#endif
/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... add */ /* RCG3AFLDL4001ZDO P-00012 end */

#endif /* FLAC__SHARE__COMPAT_H */
