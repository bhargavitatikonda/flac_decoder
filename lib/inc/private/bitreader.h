/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name    : bitreader.h
 * $Rev: 935 $
 * $Date:: 2016-06-09 16:33:32 +0900#$
 */

/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000-2009  Josh Coalson
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

#ifndef FLAC__PRIVATE__BITREADER_H
#define FLAC__PRIVATE__BITREADER_H

#include "FLAC/ordinals.h"

#if 1
#include "share/endswap.h"
#include "flacd_Lib.h"
#endif

#if 1
#define FLAC__BYTES_PER_WORD 4		/* sizeof uint32_t */
#define FLAC__BITS_PER_WORD (8 * FLAC__BYTES_PER_WORD)
#endif

/*
 * opaque structure definition
 */
#if 1 /* move from bitreader.c */
/* WATCHOUT: assembly routines rely on the order in which these fields are declared */
typedef struct {
	/* any partially-consumed word at the head will stay right-justified as bits are consumed from the left */
	/* any incomplete word at the tail will be left-justified, and bytes from the read callback are added on the right */
	FLAC__uint32 *buffer;
	unsigned capacity; /* in words */
	unsigned words; /* # of completed words in buffer */
	unsigned bytes; /* # of bytes in incomplete word at buffer[words] */
	unsigned consumed_words; /* #words ... */
	unsigned consumed_bits; /* ... + (#bits of head word) already consumed from the front of buffer */
	unsigned read_crc16; /* the running frame CRC */
	unsigned crc16_align; /* the number of bits in the current consumed word that should not be CRC'd */
} FLAC__BitReader;

/* SWAP_BE_WORD_TO_HOST swaps bytes in a uint32_t (which is always big-endian) if necessary to match host byte order */
static __inline FLAC__uint32 SWAP_BE_WORD_TO_HOST(FLAC__BitReader *br, unsigned consumed_words){
	if ( consumed_words < br->words ) {
#if WORDS_BIGENDIAN
		return br->buffer[consumed_words];
#else
#if 1
		FLAC__uint8* pValue = (FLAC__uint8*)(&br->buffer[consumed_words]);

		return (((FLAC__uint32)pValue[0] << 24) | ((FLAC__uint32)pValue[1] << 16) | ((FLAC__uint32)pValue[2] << 8) | ((FLAC__uint32)pValue[3]));
#else
		return ENDSWAP_32(br->buffer[consumed_words]);
#endif
#endif
	} else if ( consumed_words == br->words ) {
		FLAC__uint32 x;
		FLAC__uint8* pValue = (FLAC__uint8*)(&br->buffer[br->words]);
		switch ( br->bytes ) {
			case 1:
				x = (FLAC__uint32)pValue[0] << 24;
				break;
			case 2:
				x = ((FLAC__uint32)pValue[0] << 24) | ((FLAC__uint32)pValue[1] << 16) ;
				break;
			case 3:
				x = ((FLAC__uint32)pValue[0] << 24) | ((FLAC__uint32)pValue[1] << 16) | ((FLAC__uint32)pValue[2] << 8) ;
				break;
			default:
				x = (FLAC__uint32)0u;
				break;
		}
		return x;
	} else {
		return 0;
	}
}
#endif

/*
 * construction, deletion, initialization, etc functions
 */
FLAC__BitReader *FLAC__bitreader_new(FLAC__BitReader *br);
void FLAC__bitreader_init(FLAC__BitReader *br, const flacd_ioBufferConfigInfo* const pBuffConfigInfo);
FLAC__bool FLAC__bitreader_clear(FLAC__BitReader *br);

/*
 * CRC functions
 */
void FLAC__bitreader_reset_read_crc16(FLAC__BitReader *br, FLAC__uint16 seed);
FLAC__uint16 FLAC__bitreader_get_read_crc16(FLAC__BitReader *br);

/*
 * info functions
 */
#if 0
FLAC__bool FLAC__bitreader_is_consumed_byte_aligned(const FLAC__BitReader *br);
unsigned FLAC__bitreader_bits_left_for_byte_alignment(const FLAC__BitReader *br);
unsigned FLAC__bitreader_get_input_bits_unconsumed(const FLAC__BitReader *br);
#else
static __inline FLAC__bool FLAC__bitreader_is_consumed_byte_aligned(const FLAC__BitReader *br)
{
	return ((br->consumed_bits & 7) == 0);
}

static __inline unsigned FLAC__bitreader_bits_left_for_byte_alignment(const FLAC__BitReader *br)
{
	return 8 - (br->consumed_bits & 7);
}

#if 0	/* UNUSED */
static __inline unsigned FLAC__bitreader_get_input_bits_unconsumed(const FLAC__BitReader *br)
{
	return (br->words-br->consumed_words)*FLAC__BITS_PER_WORD + br->bytes*8 - br->consumed_bits;
}
#endif
#endif
#if 1
static __inline unsigned FLACD__bitreader_get_input_bytes_consumed(const FLAC__BitReader *br)
{
	return (br->consumed_words*FLAC__BYTES_PER_WORD) + (br->consumed_bits >> 3);
}
#endif

/*
 * read functions
 */

FLAC__bool FLAC__bitreader_read_raw_uint32(FLAC__BitReader *br, FLAC__uint32 *val, unsigned bits);
FLAC__bool FLAC__bitreader_read_raw_int32(FLAC__BitReader *br, FLAC__int32 *val, unsigned bits);
FLAC__bool FLAC__bitreader_read_raw_uint64(FLAC__BitReader *br, FLAC__uint64 *val, unsigned bits);
FLAC__bool FLAC__bitreader_read_uint32_little_endian(FLAC__BitReader *br, FLAC__uint32 *val); /*only for bits=32*/
FLAC__bool FLAC__bitreader_skip_bits_no_crc(FLAC__BitReader *br, unsigned bits); /* WATCHOUT: does not CRC the skipped data! */ /*@@@@ add to unit tests */
FLAC__bool FLAC__bitreader_skip_byte_block_aligned_no_crc(FLAC__BitReader *br, unsigned nvals); /* WATCHOUT: does not CRC the read data! */
FLAC__bool FLAC__bitreader_read_byte_block_aligned_no_crc(FLAC__BitReader *br, FLAC__byte *val, unsigned nvals); /* WATCHOUT: does not CRC the read data! */
FLAC__bool FLAC__bitreader_read_unary_unsigned(FLAC__BitReader *br, unsigned *val);
FLAC__bool FLAC__bitreader_read_rice_signed(FLAC__BitReader *br, int *val, unsigned parameter);
FLAC__bool FLAC__bitreader_read_rice_signed_block(FLAC__BitReader *br, int vals[], unsigned nvals, unsigned parameter);

/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... FLAC__NO_ASM delete.(lpc.h) */ /* RCG3AFLDL4001ZDO P-00015 */

#if 0 /* UNUSED */
FLAC__bool FLAC__bitreader_read_golomb_signed(FLAC__BitReader *br, int *val, unsigned parameter);
FLAC__bool FLAC__bitreader_read_golomb_unsigned(FLAC__BitReader *br, unsigned *val, unsigned parameter);
#endif
FLAC__bool FLAC__bitreader_read_utf8_uint32(FLAC__BitReader *br, FLAC__uint32 *val, FLAC__byte *raw, unsigned *rawlen);
FLAC__bool FLAC__bitreader_read_utf8_uint64(FLAC__BitReader *br, FLAC__uint64 *val, FLAC__byte *raw, unsigned *rawlen);

#endif
