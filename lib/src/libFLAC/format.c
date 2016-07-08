/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name   : format.c
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
#include "FLAC/format.h"
#include "private/format.h"

FLAC_API const unsigned FLAC__FRAME_FOOTER_CRC_LEN = 16; /* bits */

FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_TYPE_LEN = 2; /* bits */
FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN = 4; /* bits */
FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN = 4; /* bits */
FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN = 5; /* bits */
FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_RAW_LEN = 5; /* bits */

FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER = 15; /* == (1<<FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN)-1 */
FLAC_API const unsigned FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_ESCAPE_PARAMETER = 31; /* == (1<<FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN)-1 */

FLAC_API const unsigned FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN = 4; /* bits */
FLAC_API const unsigned FLAC__SUBFRAME_LPC_QLP_SHIFT_LEN = 5; /* bits */

#if 0
void FLAC__format_entropy_coding_method_partitioned_rice_contents_init(FLAC__EntropyCodingMethod_PartitionedRiceContents *object)
{
	FLAC__ASSERT(0 != object);

	object->parameters = 0;
	object->raw_bits = 0;
	object->capacity_by_order = 0;
}

FLAC__bool FLAC__format_entropy_coding_method_partitioned_rice_contents_ensure_size(FLAC__EntropyCodingMethod_PartitionedRiceContents *object, unsigned max_partition_order)
{
	FLAC__ASSERT(0 != object);

	FLAC__ASSERT(object->capacity_by_order > 0 || (0 == object->parameters && 0 == object->raw_bits));

	if(object->capacity_by_order < max_partition_order) {
		flacd_MemSet(object->raw_bits, 0, (ACMW_INT32)sizeof(unsigned)*(1 << max_partition_order));
		object->capacity_by_order = max_partition_order;
	}

	return true;
}
#endif
