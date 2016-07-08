/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/******************************************************************************
* File Name    : flacd_api.h
* $Rev: 937 $
* $Date:: 2016-06-10 12:07:52 +0900#$
* Description  : Header file for API
******************************************************************************/
#ifndef FLACD_API_H
#define FLACD_API_H

#include "protected/stream_decoder.h"
#include "private/stream_decoder.h"
#include "private/bitreader.h"
#include "flacd_Lib.h"

#define FLACD_CUSTOMERID			((ACMW_UINT32)0x00000000U)
#define FLACD_RELEASEID				((ACMW_UINT32)0x000000B1U)
#define FLACD_MAJOR					((ACMW_UINT32)0x00000002U)
#define FLACD_MINOR					((ACMW_UINT32)0x00000000U)

#define FLACD_IDSTRINGS_LENGTH		200
#ifdef RCAR_H3_M3
#define FLACD_ID_STRINGS	"FLAC Decode Software " \
							"Copyright(C) 2014 Renesas Electronics Corporation."
#else	/* other */
#define FLACD_ID_STRINGS			"IDStrings : FLAC Decoder Version 2.00"
#endif

typedef signed long					COMM_ADR_SIZE;

#define FLACD_ALIGNMENT1			((ACMW_UINT32)0x00000000U)
#define FLACD_ALIGNMENT2			((ACMW_UINT32)0x00000001U)
#define FLACD_ALIGNMENT4			((ACMW_UINT32)0x00000003U)
#define FLACD_ALIGNMENT8			((ACMW_UINT32)0x00000007U)

#define FLACD_ALIGNMENT_PTR			((ACMW_UINT32)(sizeof(COMM_ADR_SIZE) - 1))

#define FLACD_INITCFG_2CHIN			((ACMW_UINT32)0x00000000U)
#define FLACD_INITCFG_6CHIN			((ACMW_UINT32)0x00000001U)
#define FLACD_INITCFG_2CHOUT		((ACMW_UINT32)0x00000000U)
#define FLACD_INITCFG_3CHOUT		((ACMW_UINT32)0x00000001U)
#define FLACD_INITCFG_6CHOUT		((ACMW_UINT32)0x00000002U)

#define FLACD_2CHIN					((ACMW_UINT32)0x00000002U)
#define FLACD_6CHIN					((ACMW_UINT32)0x00000006U)
#define FLACD_1CHOUT				((ACMW_UINT32)0x00000001U)
#define FLACD_2CHOUT				((ACMW_UINT32)0x00000002U)
#define FLACD_3CHOUT				((ACMW_UINT32)0x00000003U)
#define FLACD_4CHOUT				((ACMW_UINT32)0x00000004U)
#define FLACD_5CHOUT				((ACMW_UINT32)0x00000005U)
#define FLACD_6CHOUT				((ACMW_UINT32)0x00000006U)

#define FLACD_CHINFO_1CH			0x0001U
#define FLACD_CHINFO_2CH			0x0006U
#define FLACD_CHINFO_3CH			0x0007U
#define FLACD_CHINFO_4CH			0x0186U
#define FLACD_CHINFO_5CH			0x0187U
#define FLACD_CHINFO_6CH			0x0587U

#define FLACD_MIN_SAMPLE_RATE		((ACMW_UINT32)8000U)
#ifdef FLACD_ENABLE_SF_192KHZ
#define FLACD_MAX_SAMPLE_RATE		((ACMW_UINT32)192000U)
#else
#define FLACD_MAX_SAMPLE_RATE		((ACMW_UINT32)96000U)
#endif

#define FLACD_MIN_INBUFF_SIZE		((ACMW_UINT32)0x00000002U)

#define FLACD_INBUFF_DATA_SIZE		((ACMW_UINT32)(FLAC__SPEC_MAX_BLOCK_SIZE * (FLAC__REFERENCE_CODEC_MAX_BITS_PER_SAMPLE >> 3)))		/* RCG3AFLDL4001ZDO P-00016 */
#define FLACD_INBUFF_HEADER_SIZE	((ACMW_UINT32)1024U)

#define FLACD_STACK_SIZE			((ACMW_UINT32)0x2000U)

#define FLACD_SYNC_CODE_BYTE		2U

#define FLACD_SHIFT_08				((ACMW_UINT32)8U)
#define FLACD_SHIFT_16				((ACMW_UINT32)16U)
#define FLACD_SHIFT_24				((ACMW_UINT32)24U)

#define FLACD_ERROR_FACTOR_ENABLE	0x0001U

typedef struct {
	ACMW_UINT32						nErrorFactor;
	ACMW_UINT16						nEnableErrorFactor;
	ACMW_UINT16						nInputChannel;
	ACMW_UINT16						nOutputChannel;
	ACMW_UINT16						nOutBitsPerSample;
	FLAC__StreamDecoder				pStreamDecoder;
	FLAC__StreamDecoderProtected	pStreamDecoderProtected;
	FLAC__StreamDecoderPrivate		pStreamDecoderPrivate;
	FLAC__BitReader					pBitReader;
} flacd_StaticArea;

typedef struct {
	FLAC__int32						Residual[FLAC__SPEC_MAX_BLOCK_SIZE];	/* RCG3AFLDL4001ZDO P-00016 */
} flacd_ScratchArea;

#endif /* FLACD_API_H */
