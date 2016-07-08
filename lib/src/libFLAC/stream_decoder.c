/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name   : stream_decoder.c
 * $Rev: 934 $
 * $Date:: 2016-06-09 12:43:04 +0900#$
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

#ifdef HAVE_CONFIG_H	/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... change "#ifdef" */ /* RCG3AFLDL4001ZDO P-00001 */
#  include <config.h>
#endif

#include "flacd_common.h"
#include "FLAC/assert.h"
#include "protected/stream_decoder.h"
#include "private/bitreader.h"
#include "private/bitmath.h"
#include "private/crc.h"
#include "private/fixed.h"
#include "private/format.h"
#include "private/lpc.h"
#include "private/macros.h"

#if 1
#include "private/stream_decoder.h"
#include "flacd_Lib.h"
#include "flacd_api.h"
#endif

/* technically this should be in an "export.c" but this is convenient enough */
FLAC_API const int FLAC_API_SUPPORTS_OGG_FLAC =
#if FLAC__HAS_OGG
	1
#else
	0
#endif
;


/***********************************************************************
 *
 * Private static data
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Private class method prototypes
 *
 ***********************************************************************/

static FLAC__bool frame_sync_(FLAC__StreamDecoder *decoder);
static FLAC__bool read_frame_(FLAC__StreamDecoder *decoder, FLAC__bool *got_a_frame, FLAC__bool do_full_decode);
static FLAC__bool read_frame_header_(FLAC__StreamDecoder *decoder);
static FLAC__bool read_subframe_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode);
static FLAC__bool read_subframe_constant_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode);
static FLAC__bool read_subframe_fixed_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, const unsigned order, FLAC__bool do_full_decode);
static FLAC__bool read_subframe_lpc_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, const unsigned order, FLAC__bool do_full_decode);
static FLAC__bool read_subframe_verbatim_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode);
static FLAC__bool read_residual_partitioned_rice_(FLAC__StreamDecoder *decoder, unsigned predictor_order, unsigned partition_order, FLAC__EntropyCodingMethod_PartitionedRiceContents *partitioned_rice_contents, FLAC__int32 *residual, FLAC__bool is_extended);
static FLAC__bool read_zero_padding_(FLAC__StreamDecoder *decoder);

/***********************************************************************
 *
 * Private class data
 *
 ***********************************************************************/

#if 0 /* move to private/stream_decoder.h */
typedef struct FLAC__StreamDecoderPrivate {
#if FLAC__HAS_OGG
	FLAC__bool is_ogg;
#endif
	/* generic 32-bit datapath: */
	void (*local_lpc_restore_signal)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	/* generic 64-bit datapath: */
	void (*local_lpc_restore_signal_64bit)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	/* for use when the signal is <= 16 bits-per-sample, or <= 15 bits-per-sample on a side channel (which requires 1 extra bit): */
	void (*local_lpc_restore_signal_16bit)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	/* for use when the signal is <= 16 bits-per-sample, or <= 15 bits-per-sample on a side channel (which requires 1 extra bit), AND order <= 8: */
	void (*local_lpc_restore_signal_16bit_order8)(const FLAC__int32 residual[], unsigned data_len, const FLAC__int32 qlp_coeff[], unsigned order, int lp_quantization, FLAC__int32 data[]);
	FLAC__bool (*local_bitreader_read_rice_signed_block)(FLAC__BitReader *br, int vals[], unsigned nvals, unsigned parameter);
	void *client_data;
	FILE *file; /* only used if FLAC__stream_decoder_init_file()/FLAC__stream_decoder_init_file() called, else NULL */
	FLAC__BitReader *input;
	FLAC__int32 *output[FLAC__MAX_CHANNELS];
	FLAC__int32 *residual[FLAC__MAX_CHANNELS]; /* WATCHOUT: these are the aligned pointers; the real pointers that should be free()'d are residual_unaligned[] below */
	FLAC__EntropyCodingMethod_PartitionedRiceContents partitioned_rice_contents[FLAC__MAX_CHANNELS];
	unsigned output_capacity, output_channels;
	FLAC__uint32 fixed_block_size, next_fixed_block_size;
	FLAC__uint64 samples_decoded;
	FLAC__bool has_stream_info, has_seek_table;
	FLAC__StreamMetadata stream_info;
	FLAC__StreamMetadata seek_table;
	FLAC__bool metadata_filter[128]; /* MAGIC number 128 == total number of metadata block types == 1 << 7 */
	FLAC__byte *metadata_filter_ids;
	size_t metadata_filter_ids_count, metadata_filter_ids_capacity; /* units for both are IDs, not bytes */
	FLAC__Frame frame;
	FLAC__bool cached; /* true if there is a byte in lookahead */
	FLAC__byte header_warmup[2]; /* contains the sync code and reserved bits */
	FLAC__byte lookahead; /* temp storage when we need to look ahead one byte in the stream */
	/* unaligned (original) pointers to allocated data */
	FLAC__int32 *residual_unaligned[FLAC__MAX_CHANNELS];
	unsigned unparseable_frame_count; /* used to tell whether we're decoding a future version of FLAC or just got a bad sync */
#if FLAC__HAS_OGG
	FLAC__bool got_a_frame; /* hack needed in Ogg FLAC seek routine to check when process_single() actually writes a frame */
#endif
} FLAC__StreamDecoderPrivate;
#endif

/***********************************************************************
 *
 * Public static class data
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Class constructor/destructor
 *
 ***********************************************************************/
FLAC_API FLAC__StreamDecoder *FLAC__stream_decoder_new(const flacd_workMemoryInfo* const pWorkMemInfo)
{
	FLAC__StreamDecoder *decoder;
	unsigned i;
	flacd_StaticArea* const pStaticStructure = (flacd_StaticArea*)pWorkMemInfo->pStatic;

	FLAC__ASSERT(sizeof(int) >= 4); /* we want to die right away if this is not true */

	decoder = &(pStaticStructure->pStreamDecoder);
	
	decoder->protected_ = &(pStaticStructure->pStreamDecoderProtected);
	
	decoder->private_ = &(pStaticStructure->pStreamDecoderPrivate);
	
	decoder->private_->input = FLAC__bitreader_new(&(pStaticStructure->pBitReader));

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		decoder->private_->output[i] = 0;
		decoder->private_->residual[i] = 0;
	}

#if 0
	for(i = 0; i < FLAC__MAX_CHANNELS; i++)
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&decoder->private_->partitioned_rice_contents[i]);
#endif

	decoder->protected_->state = FLAC__STREAM_DECODER_UNINITIALIZED;

	return decoder;
}

/***********************************************************************
 *
 * Public class methods
 *
 ***********************************************************************/

static FLAC__StreamDecoderInitStatus init_stream_internal_(
	FLAC__StreamDecoder *decoder
)
{
	FLAC__ASSERT(0 != decoder);

	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED;

	/* first default to the non-asm routines */
	decoder->private_->local_lpc_restore_signal = FLAC__lpc_restore_signal;
	decoder->private_->local_lpc_restore_signal_64bit = FLAC__lpc_restore_signal_wide;
	decoder->private_->local_lpc_restore_signal_16bit = FLAC__lpc_restore_signal;
#if 0	/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00004 start */
	decoder->private_->local_lpc_restore_signal_16bit_order8 = FLAC__lpc_restore_signal;
#endif	/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00004 end */
#if 0	/* -> Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00005 start */
	decoder->private_->local_bitreader_read_rice_signed_block = FLAC__bitreader_read_rice_signed_block;
#endif	/* <- Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00005 end */
	/* from here on, errors are fatal */

	decoder->private_->fixed_block_size = decoder->private_->next_fixed_block_size = 0;
	decoder->private_->has_stream_info = false;
	decoder->private_->cached = false;

	FLAC__stream_decoder_reset(decoder);

	return FLAC__STREAM_DECODER_INIT_STATUS_OK;
}

FLAC_API FLAC__StreamDecoderInitStatus FLAC__stream_decoder_init_stream(
	FLAC__StreamDecoder *decoder
)
{
	return init_stream_internal_(
		decoder
	);
}

FLAC_API FLAC__StreamDecoderState FLAC__stream_decoder_get_state(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->state;
}

FLAC_API unsigned FLAC__stream_decoder_get_channels(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->channels;
}

#if 0
FLAC_API FLAC__ChannelAssignment FLAC__stream_decoder_get_channel_assignment(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->channel_assignment;
}
#endif

FLAC_API unsigned FLAC__stream_decoder_get_bits_per_sample(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->bits_per_sample;
}

FLAC_API unsigned FLAC__stream_decoder_get_sample_rate(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->sample_rate;
}

FLAC_API unsigned FLAC__stream_decoder_get_blocksize(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->blocksize;
}

FLAC_API FLAC__bool FLAC__stream_decoder_flush(FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);

	decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;

	return true;
}

FLAC_API FLAC__bool FLAC__stream_decoder_reset(FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);

	if(!FLAC__stream_decoder_flush(decoder)) {
		/* above call sets the state for us */
		return false;
	}

	/*
	 * This goes in reset() and not flush() because according to the spec, a
	 * fixed-blocksize stream must stay that way through the whole stream.
	 */
	decoder->private_->fixed_block_size = decoder->private_->next_fixed_block_size = 0;

	return true;
}

FLAC__bool frame_sync_(FLAC__StreamDecoder *decoder)
{
	FLAC__uint32 x;
	FLAC__bool first = true;

	/* make sure we're byte aligned */
	if(!FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input)) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__bitreader_bits_left_for_byte_alignment(decoder->private_->input)))
			return false; /* read_callback_ sets the state for us */
	}

	while(1) {
		if(decoder->private_->cached) {
			x = (FLAC__uint32)decoder->private_->lookahead;
			decoder->private_->cached = false;
		}
		else {
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
				return false; /* read_callback_ sets the state for us */
		}
		if(x == 0xff) { /* MAGIC NUMBER for the first 8 frame sync bits */
			decoder->private_->header_warmup[0] = (FLAC__byte)x;
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
				return false; /* read_callback_ sets the state for us */

			/* we have to check if we just read two 0xff's in a row; the second may actually be the beginning of the sync code */
			/* else we have to check if the second byte is the end of a sync code */
			if(x == 0xff) { /* MAGIC NUMBER for the first 8 frame sync bits */
				decoder->private_->lookahead = (FLAC__byte)x;
				decoder->private_->cached = true;
			}
			else if(x >> 1 == 0x7c) { /* MAGIC NUMBER for the last 6 sync bits and reserved 7th bit */
				decoder->private_->header_warmup[1] = (FLAC__byte)x;
				decoder->protected_->state = FLAC__STREAM_DECODER_READ_FRAME;
				return true;
			}
		}
		if(first) {
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
			first = false;
		}
	}

	return true;
}

FLAC__bool read_frame_(FLAC__StreamDecoder *decoder, FLAC__bool *got_a_frame, FLAC__bool do_full_decode)
{
	unsigned channel;
	unsigned i;
	FLAC__int32 mid, side;
	unsigned frame_crc; /* the one we calculate from the input stream */
	FLAC__uint32 x;

#if 1
	FLAC__bool tmp_do_full_decode;
	tmp_do_full_decode = do_full_decode;
#endif

	*got_a_frame = false;

	/* init the CRC */
	frame_crc = 0;
	frame_crc = FLAC__CRC16_UPDATE(decoder->private_->header_warmup[0], frame_crc);
	frame_crc = FLAC__CRC16_UPDATE(decoder->private_->header_warmup[1], frame_crc);
	FLAC__bitreader_reset_read_crc16(decoder->private_->input, (FLAC__uint16)frame_crc);

	if(!read_frame_header_(decoder))
		return false;
	if(decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) /* means we didn't sync on a valid header */
		return true;
	for(channel = 0; channel < decoder->private_->frame.header.channels; channel++) {
		/*
		 * first figure the correct bits-per-sample of the subframe
		 */
		unsigned bps = decoder->private_->frame.header.bits_per_sample;
		switch(decoder->private_->frame.header.channel_assignment) {
			case FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT:
				/* no adjustment needed */
				break;
			case FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE:
				FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
				if(channel == 1)
					bps++;
				break;
			case FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE:
				FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
				if(channel == 0)
					bps++;
				break;
			case FLAC__CHANNEL_ASSIGNMENT_MID_SIDE:
				FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
				if(channel == 1)
					bps++;
				break;
			default:
				FLAC__ASSERT(0);
		}
		/*
		 * now read it
		 */
#if 1
		if( channel > ( decoder->private_->supported_output_channels - 1 ) ) {
			do_full_decode = false;
		}
		else {
			/* do nothing */
		}
#endif
		if(!read_subframe_(decoder, channel, bps, do_full_decode))
			return false;
		if(decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) /* means bad sync or got corruption */
			return true;
#if 1
		do_full_decode = tmp_do_full_decode;
#endif
	}
	if(!read_zero_padding_(decoder))
		return false;
	if(decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) /* means bad sync or got corruption (i.e. "zero bits" were not all zeroes) */
		return true;

	/*
	 * Read the frame CRC-16 from the footer and check
	 */
	frame_crc = FLAC__bitreader_get_read_crc16(decoder->private_->input);
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__FRAME_FOOTER_CRC_LEN))
		return false; /* read_callback_ sets the state for us */
	if(frame_crc == x) {
		if(do_full_decode) {
			/* Undo any special channel coding */
			switch(decoder->private_->frame.header.channel_assignment) {
				case FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT:
					/* do nothing */
					break;
				case FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE:
					FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
					for(i = 0; i < decoder->private_->frame.header.blocksize; i++)
						decoder->private_->output[1][i] = decoder->private_->output[0][i] - decoder->private_->output[1][i];
					break;
				case FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE:
					FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
					for(i = 0; i < decoder->private_->frame.header.blocksize; i++)
						decoder->private_->output[0][i] += decoder->private_->output[1][i];
					break;
				case FLAC__CHANNEL_ASSIGNMENT_MID_SIDE:
					FLAC__ASSERT(decoder->private_->frame.header.channels == 2);
					for(i = 0; i < decoder->private_->frame.header.blocksize; i++) {
#if 1
						mid = decoder->private_->output[0][i];
						side = decoder->private_->output[1][i];
						mid <<= 1;
						mid |= (side & 1); /* i.e. if 'side' is odd... */
						decoder->private_->output[0][i] = (mid + side) >> 1;
						decoder->private_->output[1][i] = (mid - side) >> 1;
#else
						/* OPT: without 'side' temp variable */
						mid = (decoder->private_->output[0][i] << 1) | (decoder->private_->output[1][i] & 1); /* i.e. if 'side' is odd... */
						decoder->private_->output[0][i] = (mid + decoder->private_->output[1][i]) >> 1;
						decoder->private_->output[1][i] = (mid - decoder->private_->output[1][i]) >> 1;
#endif
					}
					break;
				default:
					FLAC__ASSERT(0);
					break;
			}
		}
	}
	else {
		/* Bad frame, emit error and zero the output signal */
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH;
		if(do_full_decode) {
			for(channel = 0; channel < decoder->private_->frame.header.channels; channel++) {
				flacd_MemSet(decoder->private_->output[channel], 0, (ACMW_INT32)(sizeof(FLAC__int32) * decoder->private_->frame.header.blocksize));
			}
		}
	}

	*got_a_frame = true;

	/* we wait to update fixed_block_size until here, when we're sure we've got a proper frame and hence a correct blocksize */
	if(decoder->private_->next_fixed_block_size)
		decoder->private_->fixed_block_size = decoder->private_->next_fixed_block_size;

	/* put the latest values into the public section of the decoder instance */
	decoder->protected_->channels = decoder->private_->frame.header.channels;
	decoder->protected_->channel_assignment = decoder->private_->frame.header.channel_assignment;
	decoder->protected_->bits_per_sample = decoder->private_->frame.header.bits_per_sample;
	decoder->protected_->sample_rate = decoder->private_->frame.header.sample_rate;
	decoder->protected_->blocksize = decoder->private_->frame.header.blocksize;

	FLAC__ASSERT(decoder->private_->frame.header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER);

	decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
	return true;
}

FLAC__bool read_frame_header_(FLAC__StreamDecoder *decoder)
{
	FLAC__uint32 x;
	FLAC__uint64 xx;
	unsigned i, blocksize_hint = 0, sample_rate_hint = 0;
	FLAC__byte crc8, raw_header[16]; /* MAGIC NUMBER based on the maximum frame header size, including CRC */
	unsigned raw_header_len;
	FLAC__bool is_unparseable = false;

	FLAC__ASSERT(FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input));

	/* init the raw header with the saved bits from synchronization */
	raw_header[0] = decoder->private_->header_warmup[0];
	raw_header[1] = decoder->private_->header_warmup[1];
	raw_header_len = 2;

	/* check to make sure that reserved bit is 0 */
	if(raw_header[1] & 0x02) /* MAGIC NUMBER */
		is_unparseable = true;

	/*
	 * Note that along the way as we read the header, we look for a sync
	 * code inside.  If we find one it would indicate that our original
	 * sync was bad since there cannot be a sync code in a valid header.
	 *
	 * Three kinds of things can go wrong when reading the frame header:
	 *  1) We may have sync'ed incorrectly and not landed on a frame header.
	 *     If we don't find a sync code, it can end up looking like we read
	 *     a valid but unparseable header, until getting to the frame header
	 *     CRC.  Even then we could get a false positive on the CRC.
	 *  2) We may have sync'ed correctly but on an unparseable frame (from a
	 *     future encoder).
	 *  3) We may be on a damaged frame which appears valid but unparseable.
	 *
	 * For all these reasons, we try and read a complete frame header as
	 * long as it seems valid, even if unparseable, up until the frame
	 * header CRC.
	 */

	/*
	 * read in the raw header as bytes so we can CRC it, and parse it on the way
	 */
	for(i = 0; i < 2; i++) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		if(x == 0xff) { /* MAGIC NUMBER for the first 8 frame sync bits */
			/* if we get here it means our original sync was erroneous since the sync code cannot appear in the header */
			decoder->private_->lookahead = (FLAC__byte)x;
			decoder->private_->cached = true;
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		raw_header[raw_header_len++] = (FLAC__byte)x;
	}

	switch(x = raw_header[2] >> 4) {
		case 0:
			is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.blocksize = 192;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			decoder->private_->frame.header.blocksize = 576 << (x-2);
			break;
		case 6:
		case 7:
			blocksize_hint = x;
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			decoder->private_->frame.header.blocksize = 256 << (x-8);
			break;
		default:
			FLAC__ASSERT(0);
			break;
	}

	switch(x = raw_header[2] & 0x0f) {
		case 0:
			if(decoder->private_->has_stream_info)
				decoder->private_->frame.header.sample_rate = decoder->private_->stream_info.data.stream_info.sample_rate;
			else
				is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.sample_rate = 88200;
			break;
		case 2:
			decoder->private_->frame.header.sample_rate = 176400;
			break;
		case 3:
			decoder->private_->frame.header.sample_rate = 192000;
			break;
		case 4:
			decoder->private_->frame.header.sample_rate = 8000;
			break;
		case 5:
			decoder->private_->frame.header.sample_rate = 16000;
			break;
		case 6:
			decoder->private_->frame.header.sample_rate = 22050;
			break;
		case 7:
			decoder->private_->frame.header.sample_rate = 24000;
			break;
		case 8:
			decoder->private_->frame.header.sample_rate = 32000;
			break;
		case 9:
			decoder->private_->frame.header.sample_rate = 44100;
			break;
		case 10:
			decoder->private_->frame.header.sample_rate = 48000;
			break;
		case 11:
			decoder->private_->frame.header.sample_rate = 96000;
			break;
		case 12:
		case 13:
		case 14:
			sample_rate_hint = x;
			break;
		case 15:
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		default:
			FLAC__ASSERT(0);
	}

	x = (unsigned)(raw_header[3] >> 4);
	if(x & 8) {
		decoder->private_->frame.header.channels = 2;
		switch(x & 7) {
			case 0:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE;
				break;
			case 1:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE;
				break;
			case 2:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_MID_SIDE;
				break;
			default:
				is_unparseable = true;
				break;
		}
	}
	else {
		decoder->private_->frame.header.channels = (unsigned)x + 1;
		decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT;
	}

	switch(x = (unsigned)(raw_header[3] & 0x0e) >> 1) {
		case 0:
			if(decoder->private_->has_stream_info)
				decoder->private_->frame.header.bits_per_sample = decoder->private_->stream_info.data.stream_info.bits_per_sample;
			else
				is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.bits_per_sample = 8;
			break;
		case 2:
			decoder->private_->frame.header.bits_per_sample = 12;
			break;
		case 4:
			decoder->private_->frame.header.bits_per_sample = 16;
			break;
		case 5:
			decoder->private_->frame.header.bits_per_sample = 20;
			break;
		case 6:
			decoder->private_->frame.header.bits_per_sample = 24;
			break;
		case 3:
		case 7:
			is_unparseable = true;
			break;
		default:
			FLAC__ASSERT(0);
			break;
	}

	/* check to make sure that reserved bit is 0 */
	if(raw_header[3] & 0x01) /* MAGIC NUMBER */
		is_unparseable = true;

	/* read the frame's starting sample number (or frame number as the case may be) */
	if(
		raw_header[1] & 0x01 ||
		/*@@@ this clause is a concession to the old way of doing variable blocksize; the only known implementation is flake and can probably be removed without inconveniencing anyone */
		(decoder->private_->has_stream_info && decoder->private_->stream_info.data.stream_info.min_blocksize != decoder->private_->stream_info.data.stream_info.max_blocksize)
	) { /* variable blocksize */
		if(!FLAC__bitreader_read_utf8_uint64(decoder->private_->input, &xx, raw_header, &raw_header_len))
			return false; /* read_callback_ sets the state for us */
		if(xx == FLAC__U64L(0xffffffffffffffff)) { /* i.e. non-UTF8 code... */
			decoder->private_->lookahead = raw_header[raw_header_len-1]; /* back up as much as we can */
			decoder->private_->cached = true;
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER;
		decoder->private_->frame.header.number.sample_number = xx;
	}
	else { /* fixed blocksize */
		if(!FLAC__bitreader_read_utf8_uint32(decoder->private_->input, &x, raw_header, &raw_header_len))
			return false; /* read_callback_ sets the state for us */
		if(x == 0xffffffff) { /* i.e. non-UTF8 code... */
			decoder->private_->lookahead = raw_header[raw_header_len-1]; /* back up as much as we can */
			decoder->private_->cached = true;
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER;
		decoder->private_->frame.header.number.frame_number = x;
	}

	if(blocksize_hint) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		raw_header[raw_header_len++] = (FLAC__byte)x;
		if(blocksize_hint == 7) {
			FLAC__uint32 _x;
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &_x, 8))
				return false; /* read_callback_ sets the state for us */
			raw_header[raw_header_len++] = (FLAC__byte)_x;
			x = (x << 8) | _x;
		}
		decoder->private_->frame.header.blocksize = x+1;
	}

	if(sample_rate_hint) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		raw_header[raw_header_len++] = (FLAC__byte)x;
		if(sample_rate_hint != 12) {
			FLAC__uint32 _x;
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &_x, 8))
				return false; /* read_callback_ sets the state for us */
			raw_header[raw_header_len++] = (FLAC__byte)_x;
			x = (x << 8) | _x;
		}
		if(sample_rate_hint == 12)
			decoder->private_->frame.header.sample_rate = x*1000;
		else if(sample_rate_hint == 13)
			decoder->private_->frame.header.sample_rate = x;
		else
			decoder->private_->frame.header.sample_rate = x*10;
	}

	/* read the CRC-8 byte */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
		return false; /* read_callback_ sets the state for us */
	crc8 = (FLAC__byte)x;

	if(FLAC__crc8(raw_header, raw_header_len) != crc8) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}

	/* calculate the sample number from the frame number if needed */
	decoder->private_->next_fixed_block_size = 0;
	if(decoder->private_->frame.header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
		x = decoder->private_->frame.header.number.frame_number;
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER;
		if(decoder->private_->fixed_block_size)
			decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->fixed_block_size * (FLAC__uint64)x;
		else if(decoder->private_->has_stream_info) {
			if(decoder->private_->stream_info.data.stream_info.min_blocksize == decoder->private_->stream_info.data.stream_info.max_blocksize) {
				decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->stream_info.data.stream_info.min_blocksize * (FLAC__uint64)x;
				decoder->private_->next_fixed_block_size = decoder->private_->stream_info.data.stream_info.max_blocksize;
			}
			else
				is_unparseable = true;
		}
		else if(x == 0) {
			decoder->private_->frame.header.number.sample_number = 0;
			decoder->private_->next_fixed_block_size = decoder->private_->frame.header.blocksize;
		}
		else {
			/* can only get here if the stream has invalid frame numbering and no STREAMINFO, so assume it's not the last (possibly short) frame */
			decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->frame.header.blocksize * (FLAC__uint64)x;
		}
	}

	if(is_unparseable) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}
#if 1
	{
		FLAC__uint32 nMinBlocksize;
		nMinBlocksize = ( decoder->private_->stream_info.data.stream_info.min_blocksize == decoder->private_->stream_info.data.stream_info.max_blocksize ) ? 1 : FLAC__MIN_BLOCK_SIZE;
		if ( ( (FLAC__uint32)decoder->private_->frame.header.sample_rate > FLACD_MAX_SAMPLE_RATE ) ||
			( (FLAC__uint32)decoder->private_->frame.header.sample_rate < FLACD_MIN_SAMPLE_RATE ) ||
			( decoder->private_->frame.header.blocksize > FLAC__SPEC_MAX_BLOCK_SIZE ) ||	/* RCG3AFLDL4001ZDO P-00016 */
			( (FLAC__uint32)decoder->private_->frame.header.blocksize < nMinBlocksize ) ||
			( (FLAC__uint32)decoder->private_->frame.header.channels >  decoder->private_->supported_input_channels ) ) {
				decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
				decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNSUPPORTED_STREAM;
		}
		if ( ( decoder->protected_->channels != 0U ) &&
			( decoder->protected_->bits_per_sample != 0U ) &&
			( decoder->protected_->sample_rate != 0U ) &&
			( decoder->protected_->blocksize != 0U ) ) {
			if ( ( decoder->protected_->channels != decoder->private_->frame.header.channels ) ||
				( decoder->protected_->bits_per_sample != decoder->private_->frame.header.bits_per_sample ) ||
				( decoder->protected_->sample_rate != decoder->private_->frame.header.sample_rate ) ) {
					decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
					decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_CHANGE_HEADER;
			}
		}
	}
#endif
	return true;
}

FLAC__bool read_subframe_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode)
{
	FLAC__uint32 x;
	FLAC__bool wasted_bits;
	unsigned i;

	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8)) /* MAGIC NUMBER */
		return false; /* read_callback_ sets the state for us */

	wasted_bits = (x & 1);
	x &= 0xfe;

	if(wasted_bits) {
		unsigned u;
		if(!FLAC__bitreader_read_unary_unsigned(decoder->private_->input, &u))
			return false; /* read_callback_ sets the state for us */
		decoder->private_->frame.subframes[channel].wasted_bits = u+1;
		bps -= decoder->private_->frame.subframes[channel].wasted_bits;
	}
	else
		decoder->private_->frame.subframes[channel].wasted_bits = 0;

	/*
	 * Lots of magic numbers here
	 */
	if(x & 0x80) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}
	else if(x == 0) {
		if(!read_subframe_constant_(decoder, channel, bps, do_full_decode))
			return false;
	}
	else if(x == 2) {
		if(!read_subframe_verbatim_(decoder, channel, bps, do_full_decode))
			return false;
	}
	else if(x < 16) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}
	else if(x <= 24) {
		if(!read_subframe_fixed_(decoder, channel, bps, (x>>1)&7, do_full_decode))
			return false;
		if(decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) /* means bad sync or got corruption */
			return true;
	}
	else if(x < 64) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}
	else {
		if(!read_subframe_lpc_(decoder, channel, bps, ((x>>1)&31)+1, do_full_decode))
			return false;
		if(decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) /* means bad sync or got corruption */
			return true;
	}

	if(wasted_bits && do_full_decode) {
		x = decoder->private_->frame.subframes[channel].wasted_bits;
		for(i = 0; i < decoder->private_->frame.header.blocksize; i++)
			decoder->private_->output[channel][i] <<= x;
	}

	return true;
}

FLAC__bool read_subframe_constant_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode)
{
	FLAC__Subframe_Constant *subframe = &decoder->private_->frame.subframes[channel].data.constant;
	FLAC__int32 x;
	unsigned i;
	FLAC__int32 *output = decoder->private_->output[channel];

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_CONSTANT;

	if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &x, bps))
		return false; /* read_callback_ sets the state for us */

	subframe->value = x;

	/* decode the subframe */
	if(do_full_decode) {
		for(i = 0; i < decoder->private_->frame.header.blocksize; i++)
			output[i] = x;
	}

	return true;
}

FLAC__bool read_subframe_fixed_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, const unsigned order, FLAC__bool do_full_decode)
{
	FLAC__Subframe_Fixed *subframe = &decoder->private_->frame.subframes[channel].data.fixed;
	FLAC__int32 i32;
	FLAC__uint32 u32;
	unsigned u;

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_FIXED;

	subframe->residual = decoder->private_->residual[channel] + order;
	subframe->order = order;

	/* read warm-up samples */
	for(u = 0; u < order; u++) {
		if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &i32, bps))
			return false; /* read_callback_ sets the state for us */
		subframe->warmup[u] = i32;
	}

	/* read entropy coding method info */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &u32, FLAC__ENTROPY_CODING_METHOD_TYPE_LEN))
		return false; /* read_callback_ sets the state for us */
	subframe->entropy_coding_method.type = (FLAC__EntropyCodingMethodType)u32;
	switch(subframe->entropy_coding_method.type) {
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE:
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2:
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &u32, FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN))
				return false; /* read_callback_ sets the state for us */
			subframe->entropy_coding_method.data.partitioned_rice.order = u32;
			subframe->entropy_coding_method.data.partitioned_rice.contents = &decoder->private_->partitioned_rice_contents[channel];
			break;
		default:
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
	}

	/* read residual */
	switch(subframe->entropy_coding_method.type) {
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE:
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2:
			if(!read_residual_partitioned_rice_(decoder, order, subframe->entropy_coding_method.data.partitioned_rice.order, &decoder->private_->partitioned_rice_contents[channel], decoder->private_->residual[channel] + order, /*is_extended=*/subframe->entropy_coding_method.type == FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2))
				return false;
			break;
		default:
			FLAC__ASSERT(0);
	}

	/* decode the subframe */
	if(do_full_decode) {
		flacd_MemCpy(decoder->private_->output[channel], subframe->warmup, (ACMW_INT32)(sizeof(FLAC__int32) * order));
		FLAC__fixed_restore_signal(decoder->private_->residual[channel] + order, decoder->private_->frame.header.blocksize-order, order, decoder->private_->output[channel]+order);
	}

	return true;
}

FLAC__bool read_subframe_lpc_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, const unsigned order, FLAC__bool do_full_decode)
{
	FLAC__Subframe_LPC *subframe = &decoder->private_->frame.subframes[channel].data.lpc;
	FLAC__int32 i32;
	FLAC__uint32 u32;
	unsigned u;

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_LPC;

	subframe->residual = decoder->private_->residual[channel] + order;	/* order - renesas */
	subframe->order = order;

	/* read warm-up samples */
	for(u = 0; u < order; u++) {
		if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &i32, bps))
			return false; /* read_callback_ sets the state for us */
		subframe->warmup[u] = i32;
	}

	/* read qlp coeff precision */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &u32, FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN))
		return false; /* read_callback_ sets the state for us */
	if(u32 == (1u << FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN) - 1) {
		decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}
	subframe->qlp_coeff_precision = u32+1;

	/* read qlp shift */
	if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &i32, FLAC__SUBFRAME_LPC_QLP_SHIFT_LEN))
		return false; /* read_callback_ sets the state for us */
	subframe->quantization_level = i32;

	/* read quantized lp coefficiencts */
	for(u = 0; u < order; u++) {
		if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &i32, subframe->qlp_coeff_precision))
			return false; /* read_callback_ sets the state for us */
		subframe->qlp_coeff[u] = i32;
	}

	/* read entropy coding method info */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &u32, FLAC__ENTROPY_CODING_METHOD_TYPE_LEN))
		return false; /* read_callback_ sets the state for us */
	subframe->entropy_coding_method.type = (FLAC__EntropyCodingMethodType)u32;
	switch(subframe->entropy_coding_method.type) {
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE:
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2:
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &u32, FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN))
				return false; /* read_callback_ sets the state for us */
			subframe->entropy_coding_method.data.partitioned_rice.order = u32;
			subframe->entropy_coding_method.data.partitioned_rice.contents = &decoder->private_->partitioned_rice_contents[channel];
			break;
		default:
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
	}

	/* read residual */
	switch(subframe->entropy_coding_method.type) {
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE:
		case FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2:
			if(!read_residual_partitioned_rice_(decoder, order, subframe->entropy_coding_method.data.partitioned_rice.order, &decoder->private_->partitioned_rice_contents[channel], decoder->private_->residual[channel] + order, /*is_extended=*/subframe->entropy_coding_method.type == FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2))
				return false;
			break;
		default:
			FLAC__ASSERT(0);
	}

	/* decode the subframe */
	if(do_full_decode) {
		flacd_MemCpy(decoder->private_->output[channel], subframe->warmup, (ACMW_INT32)(sizeof(FLAC__int32) * order));
		/*@@@@@@ technically not pessimistic enough, should be more like
		if( (FLAC__uint64)order * ((((FLAC__uint64)1)<<bps)-1) * ((1<<subframe->qlp_coeff_precision)-1) < (((FLAC__uint64)-1) << 32) )
		*/
		if(bps + subframe->qlp_coeff_precision + FLAC__bitmath_ilog2(order) <= 32)
			if(bps <= 16 && subframe->qlp_coeff_precision <= 16)	/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... delete */ /* RCG3AFLDL4001ZDO P-00004 */
				decoder->private_->local_lpc_restore_signal_16bit(decoder->private_->residual[channel] + order, decoder->private_->frame.header.blocksize-order, subframe->qlp_coeff, order, subframe->quantization_level, decoder->private_->output[channel]+order);
			else
				decoder->private_->local_lpc_restore_signal(decoder->private_->residual[channel] + order, decoder->private_->frame.header.blocksize-order, subframe->qlp_coeff, order, subframe->quantization_level, decoder->private_->output[channel]+order);
		else
			decoder->private_->local_lpc_restore_signal_64bit(decoder->private_->residual[channel] + order, decoder->private_->frame.header.blocksize-order, subframe->qlp_coeff, order, subframe->quantization_level, decoder->private_->output[channel]+order);
	}

	return true;
}

FLAC__bool read_subframe_verbatim_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode)
{
	FLAC__Subframe_Verbatim *subframe = &decoder->private_->frame.subframes[channel].data.verbatim;
	FLAC__int32 x, *residual = decoder->private_->residual[channel];
	unsigned i;

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_VERBATIM;

	subframe->data = residual;

	for(i = 0; i < decoder->private_->frame.header.blocksize; i++) {
		if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &x, bps))
			return false; /* read_callback_ sets the state for us */
		residual[i] = x;
	}

	/* decode the subframe */
	if(do_full_decode)
		flacd_MemCpy(decoder->private_->output[channel], subframe->data, (ACMW_INT32)(sizeof(FLAC__int32) * decoder->private_->frame.header.blocksize));

	return true;
}

FLAC__bool read_residual_partitioned_rice_(FLAC__StreamDecoder *decoder, unsigned predictor_order, unsigned partition_order, FLAC__EntropyCodingMethod_PartitionedRiceContents *partitioned_rice_contents, FLAC__int32 *residual, FLAC__bool is_extended)
{
	FLAC__uint32 rice_parameter;
	int i;
	unsigned partition, sample, u;
	const unsigned partitions = 1u << partition_order;
	const unsigned partition_samples = partition_order > 0? decoder->private_->frame.header.blocksize >> partition_order : decoder->private_->frame.header.blocksize - predictor_order;
	const unsigned plen = is_extended? FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN : FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN;
	const unsigned pesc = is_extended? FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_ESCAPE_PARAMETER : FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER;

	/* sanity checks */
	if(partition_order == 0) {
		if(decoder->private_->frame.header.blocksize < predictor_order) {
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			/* We have received a potentially malicious bit stream. All we can do is error out to avoid a heap overflow. */
			return false;								/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... change */ /* RCG3AFLDL4001ZDO P-00006 */
		}
	}
	else {
		if(partition_samples < predictor_order) {
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			/* We have received a potentially malicious bit stream. All we can do is error out to avoid a heap overflow. */
			return false;								/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... change */ /* RCG3AFLDL4001ZDO P-00006 */
		}
	}

#if 0
	if(!FLAC__format_entropy_coding_method_partitioned_rice_contents_ensure_size(partitioned_rice_contents, flac_max(6u, partition_order))) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}
#endif

	sample = 0;
	for(partition = 0; partition < partitions; partition++) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &rice_parameter, plen))
			return false; /* read_callback_ sets the state for us */
#if 0
		partitioned_rice_contents->parameters[partition] = rice_parameter;
#endif
		if(rice_parameter < pesc) {
#if 0
			partitioned_rice_contents->raw_bits[partition] = 0;
#endif
			u = (partition_order == 0 || partition > 0)? partition_samples : partition_samples - predictor_order;
			/* Reference Program : FLAC 1.3.1 (25-Nov-2014) ... change */ /* RCG3AFLDL4001ZDO P-00007 */
			if(!FLAC__bitreader_read_rice_signed_block(decoder->private_->input, residual + sample, u, rice_parameter))
				return false; /* read_callback_ sets the state for us */
			sample += u;
		}
		else {
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &rice_parameter, FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_RAW_LEN))
				return false; /* read_callback_ sets the state for us */
#if 0
			partitioned_rice_contents->raw_bits[partition] = rice_parameter;
#endif
			for(u = (partition_order == 0 || partition > 0)? 0 : predictor_order; u < partition_samples; u++, sample++) {
				if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &i, rice_parameter))
					return false; /* read_callback_ sets the state for us */
				residual[sample] = i;
			}
		}
	}

	return true;
}

FLAC__bool read_zero_padding_(FLAC__StreamDecoder *decoder)
{
	if(!FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input)) {
		FLAC__uint32 zero = 0;
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &zero, FLAC__bitreader_bits_left_for_byte_alignment(decoder->private_->input)))
			return false; /* read_callback_ sets the state for us */
		if(zero != 0) {
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		}
	}
	return true;
}

#if 1
FLAC_API void FLAC__stream_decoder_set_supported_channels(FLAC__StreamDecoder* const decoder, unsigned const input_channels, unsigned const output_channels)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	
	decoder->private_->supported_input_channels = input_channels;
	decoder->private_->supported_output_channels = output_channels;

	return ;
}

FLAC_API void FLAC__stream_decoder_set_streaminfo(FLAC__StreamDecoder* const decoder, const flacd_decConfigInfo* const pDecConfigInfo )
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	
	if ( pDecConfigInfo != NULL ) {
		decoder->private_->stream_info.data.stream_info.min_blocksize = pDecConfigInfo->nMinBlockSize;
		decoder->private_->stream_info.data.stream_info.max_blocksize = pDecConfigInfo->nMaxBlockSize;
		decoder->private_->stream_info.data.stream_info.min_framesize = pDecConfigInfo->nMinFrameSize;
		decoder->private_->stream_info.data.stream_info.max_framesize = pDecConfigInfo->nMaxFrameSize;
		decoder->private_->stream_info.data.stream_info.sample_rate = pDecConfigInfo->nSampleRate;
		decoder->private_->stream_info.data.stream_info.channels = pDecConfigInfo->nChannels;
		decoder->private_->stream_info.data.stream_info.bits_per_sample = pDecConfigInfo->nBitsPerSample;
		decoder->private_->has_stream_info = true;
	}
	return ;
}

FLAC_API FLAC__StreamDecoderErrorStatus FLAC__stream_decoder_get_error_state(const FLAC__StreamDecoder* const decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->error_state;
}

#if 0
FLAC_API void FLAC__stream_decoder_allocate_rice_contents(FLAC__StreamDecoder* const decoder, unsigned ** const pParameters, unsigned ** const pRaw_bits)
{
	unsigned i;
	
	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		if ( ( pParameters != NULL ) && ( pParameters[i] != NULL ) && ( pRaw_bits != NULL ) && ( pRaw_bits[i] != NULL )) {
			decoder->private_->partitioned_rice_contents[i].parameters = pParameters[i];
			decoder->private_->partitioned_rice_contents[i].raw_bits = pRaw_bits[i];
		} else {
			decoder->private_->partitioned_rice_contents[i].parameters = (unsigned int*)NULL;
			decoder->private_->partitioned_rice_contents[i].raw_bits = (unsigned int*)NULL;
		}
	}

	return ;
}
#endif

FLAC_API void FLAC__stream_decoder_allocate_residual(FLAC__StreamDecoder* const decoder, FLAC__int32** const pResidual)
{
	unsigned i;
	
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		if ( ( pResidual != NULL ) && ( pResidual[i] != NULL ) ) {
			decoder->private_->residual[i] = pResidual[i];
		} else {
			decoder->private_->residual[i] = (FLAC__int32*)NULL;
		}
	}

	return ;
}

FLAC_API void FLAC__stream_decoder_allocate_output(FLAC__StreamDecoder* const decoder, FLAC__int32** const pOutput)
{
	unsigned i;
	
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		if ( ( pOutput != NULL ) && ( pOutput[i] != NULL ) ) {
			decoder->private_->output[i] = pOutput[i];
		} else {
			decoder->private_->output[i] = (FLAC__int32*)NULL;
		}
	}

	return ;
}

FLAC_API FLAC__bool FLAC__stream_decoder_process_frame_sync(FLAC__StreamDecoder* const decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);

	switch(decoder->protected_->state) {
		case FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC:
			decoder->protected_->error_state = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
			if(!frame_sync_(decoder))
				return false;
			else
				return true;
		case FLAC__STREAM_DECODER_SEARCH_FOR_METADATA:
		case FLAC__STREAM_DECODER_READ_METADATA:
		case FLAC__STREAM_DECODER_READ_FRAME:
		case FLAC__STREAM_DECODER_END_OF_STREAM:
		case FLAC__STREAM_DECODER_ABORTED:
			return true;
		default:
			FLAC__ASSERT(0);
			return false;
	}
}

FLAC_API FLAC__bool FLAC__stream_decoder_process_read_frame(FLAC__StreamDecoder* const decoder, FLAC__bool* const got_a_frame)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);

	switch(decoder->protected_->state) {
		case FLAC__STREAM_DECODER_READ_FRAME:
			if(!read_frame_(decoder, got_a_frame, /*do_full_decode=*/true))
				return false;
			else
				return true;
		case FLAC__STREAM_DECODER_SEARCH_FOR_METADATA:
		case FLAC__STREAM_DECODER_READ_METADATA:
		case FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC:
		case FLAC__STREAM_DECODER_END_OF_STREAM:
		case FLAC__STREAM_DECODER_ABORTED:
			return true;
		default:
			FLAC__ASSERT(0);
			return false;
	}
}
#endif

