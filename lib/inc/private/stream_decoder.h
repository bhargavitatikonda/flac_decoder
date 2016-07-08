/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name    : stream_decoder.h
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

#ifndef FLAC__PRIVATE__STREAM_DECODER_H
#define FLAC__PRIVATE__STREAM_DECODER_H

#include "FLAC/stream_decoder.h"
#include "private/bitreader.h"

typedef struct FLAC__StreamDecoderPrivate {
	/* generic 32-bit datapath: */
	void (*local_lpc_restore_signal)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	/* generic 64-bit datapath: */
	void (*local_lpc_restore_signal_64bit)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	/* for use when the signal is <= 16 bits-per-sample, or <= 15 bits-per-sample on a side channel (which requires 1 extra bit): */
	void (*local_lpc_restore_signal_16bit)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
#if 0	/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00004 start */
	/* for use when the signal is <= 16 bits-per-sample, or <= 15 bits-per-sample on a side channel (which requires 1 extra bit), AND order <= 8: */
	void (*local_lpc_restore_signal_16bit_order8)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
#endif	/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00004 end */
#if 0	/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00005 start */
	FLAC__bool (*local_bitreader_read_rice_signed_block)(FLAC__BitReader *br, int vals[], unsigned nvals, unsigned parameter);
#endif	/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00005 end */
	FLAC__BitReader *input;
	FLAC__int32 *output[FLAC__MAX_CHANNELS];
	FLAC__int32 *residual[FLAC__MAX_CHANNELS]; /* WATCHOUT: these are the aligned pointers; the real pointers that should be free()'d are residual_unaligned[] below */
	FLAC__EntropyCodingMethod_PartitionedRiceContents partitioned_rice_contents[FLAC__MAX_CHANNELS];
	FLAC__uint32 fixed_block_size, next_fixed_block_size;
	FLAC__bool has_stream_info;
	FLAC__StreamMetadata stream_info;
	FLAC__Frame frame;
	FLAC__bool cached; /* true if there is a byte in lookahead */
	FLAC__byte header_warmup[2]; /* contains the sync code and reserved bits */
	FLAC__byte lookahead; /* temp storage when we need to look ahead one byte in the stream */
	/* unaligned (original) pointers to allocated data */
#if 1
	FLAC__uint32 supported_input_channels;
	FLAC__uint32 supported_output_channels;
#endif
} FLAC__StreamDecoderPrivate;

#endif
