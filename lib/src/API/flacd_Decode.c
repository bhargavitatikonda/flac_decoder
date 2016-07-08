/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_Decode.c
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description : Source file for API
******************************************************************************/

#include "flacd_common.h"
#include "FLAC/stream_decoder.h"
#include "flacd_Lib.h"
#include "flacd_api.h"

static void output16pcm( ACMW_INT16*		const pDstBuff,
						 const FLAC__int32*	const pSrcBuff,
						 const ACMW_UINT32	block_length,
						 const ACMW_INT32	nShiftBits );
static void output24pcm( ACMW_INT32*		const pDstBuff,
						 const FLAC__int32*	const pSrcBuff,
						 const ACMW_UINT32	block_length,
						 const ACMW_INT32	nShiftBits );

/*==========================================================================
 [Function Name]
   flacd_Decode

 [Description]
   FLAC Decoder of the FLACD SW.IP. This is a top level wrapper.

 [Prototype]
   INT32  flacd_Decode(
           flacd_workMemoryInfo *pWorkMemInfo,
           flacd_decConfigInfo *pDecConfigInfo,
           flacd_ioBufferConfigInfo *pBuffConfigInfo,
           flacd_decStatusInfo *pDecStatusInfo,
           flacd_ioBufferStatusInfo *pBuffStatusInfo)

 [Argument]
   const flacd_workMemoryInfo *       pWorkMemInfo
           Pointer to Work Memory Information Structure
   const flacd_decConfigInfo *        pDecConfigInfo
           Pointer to Decoder Configuration Information Structure
   const flacd_ioBufferConfigInfo *   pBuffConfigInfo
           Pointer to Buffer Configuration Information Structure
   const flacd_decStatusInfo *        pDecStatusInfo
           Pointer to Decoder Status Information Structure
   const flacd_ioBufferStatusInfo *   pBuffStatusInfo
           Pointer to Buffer Status Information Structure

 [Return]
   INT32 Error Code
           FLACD_RESULT_OK      : Normal
           FLACD_RESULT_NG      : Error (+ Error Factor)
           FLACD_RESULT_WARNING : Warning Error
           FLACD_RESULT_FATAL   : Error (none Error Factor)

 [Note]
   None
==========================================================================*/
ACMW_INT32 flacd_Decode( const flacd_workMemoryInfo*		const pWorkMemInfo,
						 const flacd_decConfigInfo*			const pDecConfigInfo,
						 const flacd_ioBufferConfigInfo*	const pBuffConfigInfo,
						 flacd_decStatusInfo*				const pDecStatusInfo,
						 flacd_ioBufferStatusInfo*			const pBuffStatusInfo )
{
	flacd_StaticArea *pStaticStructure;
	flacd_ScratchArea *pScratchStructure[FLAC__MAX_CHANNELS];
	FLAC__int32 *pResidual[FLAC__MAX_CHANNELS];
	FLAC__bool	ret, got_a_frame;
	FLAC__StreamDecoderState get_state;
	FLAC__uint32	i;
	FLAC__uint32	frameStartUsedDataSize = 0;
	
	/* Argument Check */
	if (pWorkMemInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		return FLACD_RESULT_FATAL;
	} /* end if */
	
	if ((pWorkMemInfo->pStatic == NULL) || ((((ACMW_UINT32)(pWorkMemInfo->pStatic)) & FLACD_ALIGNMENT_PTR) != (ACMW_UINT32)0U)) {
		return FLACD_RESULT_FATAL;
	} /* end if */
	
	pStaticStructure = (flacd_StaticArea*)pWorkMemInfo->pStatic;
	/* Set ErrorCode */
	pStaticStructure->nErrorFactor = FLACD_ERR_NONE;
	
	if (pBuffStatusInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	pBuffStatusInfo->nInBuffUsedDataSize = (ACMW_UINT32)0U;
	pBuffStatusInfo->nOutBuffUsedDataSize = (ACMW_UINT32)0U;
	
	if (pBuffConfigInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	pBuffStatusInfo->pInBuffLast = pBuffConfigInfo->pInBuffStart;
	
	if ((pWorkMemInfo->pScratch == NULL) || ((((ACMW_UINT32)(pWorkMemInfo->pScratch)) & FLACD_ALIGNMENT_PTR) != (ACMW_UINT32)0U) ||
		(pDecConfigInfo == NULL) ||		/* [2015.09.04] -> Alignment do not check for structure pointer */
		(pDecStatusInfo == NULL) ) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	/* Fill Status Info */
	flacd_MemSet(pDecStatusInfo, 0, sizeof(flacd_decStatusInfo));
	
	/* Check Sequence */
	get_state = pStaticStructure->pStreamDecoderProtected.state;
	if ( ( get_state != FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC ) &&
		 ( get_state != FLAC__STREAM_DECODER_READ_FRAME ) ) {
		pStaticStructure->nErrorFactor = FLACD_ERR_SEQUENCE;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if ((pBuffConfigInfo->pInBuffStart  == NULL) ||
		(pBuffConfigInfo->pOutBuffStart == NULL) ||		/* [2015.09.04] -> Alignment do not check */
		(pBuffStatusInfo->pOutBuffLast  == NULL) ) {	/* [2015.09.04] -> Alignment do not check */
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if ( pStaticStructure->nOutBitsPerSample == 0x0000u ) {
		for (i = 0; i < (uint32_t)pStaticStructure->nOutputChannel; i++) {
			if ((((ACMW_UINT32)(pBuffConfigInfo->pOutBuffStart[i])) & FLACD_ALIGNMENT2) != (ACMW_UINT32)0U) {
				pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
				return FLACD_RESULT_NG;
			} /* end if */
		}
	}
	else {
		for (i = 0; i < (uint32_t)pStaticStructure->nOutputChannel; i++) {
			if ((((ACMW_UINT32)(pBuffConfigInfo->pOutBuffStart[i])) & FLACD_ALIGNMENT4) != (ACMW_UINT32)0U) {
				pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
				return FLACD_RESULT_NG;
			} /* end if */
		}
	} /* end if */
	
	/* Check BuffConfig Value */
	if (pBuffConfigInfo->nInBuffSetDataSize < FLACD_MIN_INBUFF_SIZE) {
		pStaticStructure->nErrorFactor = FLACD_ERR_SHORT_INPUT_DATA;
		return FLACD_RESULT_NG;
	} /* end if */

	/* Check DecConfig Value */
	if ((uint32_t)pDecConfigInfo->nMaxBlockSize > FLAC__SPEC_MAX_BLOCK_SIZE) {	/* RCG3AFLDL4001ZDO P-00016 */
		pStaticStructure->nErrorFactor = FLACD_ERR_NOT_SUPPORTED_DATA;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if ((pDecConfigInfo->nSampleRate > FLACD_MAX_SAMPLE_RATE) || (pDecConfigInfo->nSampleRate < FLACD_MIN_SAMPLE_RATE)) {
		pStaticStructure->nErrorFactor = FLACD_ERR_NOT_SUPPORTED_DATA;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if ((uint32_t)pDecConfigInfo->nChannels > (uint32_t)pStaticStructure->nInputChannel) {
		pStaticStructure->nErrorFactor = FLACD_ERR_NOT_SUPPORTED_DATA;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if (((uint32_t)pDecConfigInfo->nBitsPerSample > FLAC__REFERENCE_CODEC_MAX_BITS_PER_SAMPLE) ||
		((uint32_t)pDecConfigInfo->nBitsPerSample < FLAC__MIN_BITS_PER_SAMPLE)) {
		pStaticStructure->nErrorFactor = FLACD_ERR_NOT_SUPPORTED_DATA;
		return FLACD_RESULT_NG;
	} /* end if */
	
	for( i = 0; i < FLAC__MAX_CHANNELS; i++ ) {
		if ( i < pStaticStructure->nInputChannel ) {
			pScratchStructure[i] = (flacd_ScratchArea*)((ACMW_UINT8 *)pWorkMemInfo->pScratch + (sizeof(flacd_ScratchArea) * i));
			pResidual[i] = &(pScratchStructure[i]->Residual[0]);
			flacd_MemSet((void*)pResidual[i], 0u, (ACMW_INT32)(pDecConfigInfo->nMaxBlockSize * sizeof(FLAC__int32)));
		}
		else {
			pScratchStructure[i] = NULL;
			pResidual[i] = NULL;
		}
	}
	
	/* Set DecConfig Value */
	FLAC__stream_decoder_set_streaminfo( &(pStaticStructure->pStreamDecoder), pDecConfigInfo );
	
	/* Set BuffConfig Value */
	FLAC__bitreader_init(&(pStaticStructure->pBitReader), pBuffConfigInfo);
	FLAC__stream_decoder_allocate_residual( &(pStaticStructure->pStreamDecoder), pResidual);
	FLAC__stream_decoder_allocate_output( &(pStaticStructure->pStreamDecoder), pResidual);

	/* Check Header Sync */
	ret = FLAC__stream_decoder_process_frame_sync( &(pStaticStructure->pStreamDecoder) );
	/* Update Input Buffer Infomation */
	pBuffStatusInfo->nInBuffUsedDataSize = FLACD__bitreader_get_input_bytes_consumed(&(pStaticStructure->pBitReader));
	pBuffStatusInfo->pInBuffLast = pBuffConfigInfo->pInBuffStart + pBuffStatusInfo->nInBuffUsedDataSize;
	if (pBuffStatusInfo->nInBuffUsedDataSize >= FLACD_SYNC_CODE_BYTE) {
		frameStartUsedDataSize = pBuffStatusInfo->nInBuffUsedDataSize - FLACD_SYNC_CODE_BYTE;
	}
	if (ret == false) {
		pStaticStructure->nErrorFactor = FLACD_ERR_LOST_SYNC;
		return FLACD_RESULT_NG;
	}
	if ( FLAC__STREAM_DECODER_READ_FRAME != FLAC__stream_decoder_get_state( &pStaticStructure->pStreamDecoder ) ) {
		pStaticStructure->nErrorFactor = FLACD_ERR_LOST_SYNC;
		return FLACD_RESULT_NG;
	}
	/* Decode Frame */
	got_a_frame = false;
	ret = FLAC__stream_decoder_process_read_frame( &(pStaticStructure->pStreamDecoder), &got_a_frame );
	/* Update Input Buffer Infomation */
	pBuffStatusInfo->nInBuffUsedDataSize = FLACD__bitreader_get_input_bytes_consumed(&(pStaticStructure->pBitReader));
	pBuffStatusInfo->pInBuffLast = pBuffConfigInfo->pInBuffStart + pBuffStatusInfo->nInBuffUsedDataSize;
	if( got_a_frame == true ){
		/* Check Output Buffer Parameter */
		ACMW_UINT32	nOutputSize;
		const ACMW_UINT16 sChannelInfo[FLACD_6CHOUT] = {
			(ACMW_UINT16)FLACD_CHINFO_1CH,
			(ACMW_UINT16)FLACD_CHINFO_2CH,
			(ACMW_UINT16)FLACD_CHINFO_3CH,
			(ACMW_UINT16)FLACD_CHINFO_4CH,
			(ACMW_UINT16)FLACD_CHINFO_5CH,
			(ACMW_UINT16)FLACD_CHINFO_6CH
		};

		if ( pStaticStructure->nOutBitsPerSample == 0x0000u ) {
			nOutputSize = (ACMW_UINT32)(FLAC__stream_decoder_get_blocksize(&(pStaticStructure->pStreamDecoder)) * sizeof(ACMW_INT16));
		} else {
			nOutputSize = (ACMW_UINT32)(FLAC__stream_decoder_get_blocksize(&(pStaticStructure->pStreamDecoder)) * sizeof(ACMW_INT32));
		} /* end if */
		if ( pBuffConfigInfo->nOutBuffSize < nOutputSize ) {
			pStaticStructure->nErrorFactor = FLACD_ERR_PARAMETER;
			return FLACD_RESULT_NG;
		} /* end if */

		/* Set to Decoder Status Information Structure */
		pDecStatusInfo->nSampleRate = FLAC__stream_decoder_get_sample_rate(&(pStaticStructure->pStreamDecoder));
		pDecStatusInfo->nDecodedSamples = FLAC__stream_decoder_get_blocksize(&(pStaticStructure->pStreamDecoder));
		pDecStatusInfo->nChannels = (ACMW_UINT16)FLAC__stream_decoder_get_channels(&(pStaticStructure->pStreamDecoder));
		pDecStatusInfo->nBitsPerSample = (ACMW_UINT16)FLAC__stream_decoder_get_bits_per_sample(&(pStaticStructure->pStreamDecoder));
		pBuffStatusInfo->nOutBuffUsedDataSize = nOutputSize ;

		ret = FLAC__stream_decoder_get_error_state(&(pStaticStructure->pStreamDecoder));
		if ( ret == FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH ) {
			/* Set to Status Infomation about Channel Infomation */
			pDecStatusInfo->nChannelInfo = sChannelInfo[(ACMW_UINT32)pDecStatusInfo->nChannels-(ACMW_UINT32)1U];

			/* Set Output Mute Data */
			for (i = 0; i < (uint32_t)pStaticStructure->nOutputChannel; i++) {
				if( pBuffConfigInfo->pOutBuffStart[i] != NULL ){
					if ( pStaticStructure->nOutBitsPerSample == 0x0000u ) {
						flacd_MemSet(pBuffConfigInfo->pOutBuffStart[i], 0u, (ACMW_INT32)(pDecStatusInfo->nDecodedSamples * sizeof(ACMW_INT16)));
					} else {
						flacd_MemSet(pBuffConfigInfo->pOutBuffStart[i], 0u, (ACMW_INT32)(pDecStatusInfo->nDecodedSamples * sizeof(ACMW_INT32)));
					} /* end if */
					pBuffStatusInfo->pOutBuffLast[i] = (void*)((ACMW_UINT8 *)pBuffConfigInfo->pOutBuffStart[i] + pBuffStatusInfo->nOutBuffUsedDataSize);
				}
				else{
					pBuffStatusInfo->pOutBuffLast[i] = NULL;
				} /* end if */
			} /* end for */

			pStaticStructure->nErrorFactor = FLACD_ERR_CRC;
			return FLACD_RESULT_WARNING;
		} /* end if */
		else {
			/* if the number of input channels is 4 and The number of output channels is 3, only the left and the right output */
			if( ( (ACMW_UINT32)pDecStatusInfo->nChannels == FLACD_4CHOUT ) && ( (ACMW_UINT32)pStaticStructure->nOutputChannel == FLACD_3CHOUT ) ) {
				flacd_MemSet( pResidual[FLACD_3CHOUT-(ACMW_UINT32)1U], 0u, (ACMW_INT32)(pDecStatusInfo->nDecodedSamples * sizeof(FLAC__int32)) );
				pDecStatusInfo->nChannels = (ACMW_UINT16)FLACD_2CHOUT;
			} /* end if */
					
			/* Set to Status Infomation about Channel Infomation */
			pDecStatusInfo->nChannelInfo = sChannelInfo[(ACMW_UINT32)pDecStatusInfo->nChannels-(ACMW_UINT32)1U];

			/* Set Output Data */
			for (i = 0; i < (uint32_t)pStaticStructure->nOutputChannel; i++) {
				if( pBuffConfigInfo->pOutBuffStart[i] != NULL ){
					if ( pStaticStructure->nOutBitsPerSample == 0x0000u ) {
						const ACMW_INT32 nShiftBits = (ACMW_INT32)16 - (ACMW_INT32)pDecStatusInfo->nBitsPerSample;
						output16pcm( (ACMW_INT16*)pBuffConfigInfo->pOutBuffStart[i], pResidual[i], pDecStatusInfo->nDecodedSamples, nShiftBits );
					} else {
						const ACMW_INT32 nShiftBits = (ACMW_INT32)32 - (ACMW_INT32)pDecStatusInfo->nBitsPerSample;
						output24pcm( (ACMW_INT32*)pBuffConfigInfo->pOutBuffStart[i], pResidual[i], pDecStatusInfo->nDecodedSamples, nShiftBits );
					} /* end if */
					pBuffStatusInfo->pOutBuffLast[i] = (void*)((ACMW_UINT8 *)pBuffConfigInfo->pOutBuffStart[i] + pBuffStatusInfo->nOutBuffUsedDataSize);
				}
				else{
					pBuffStatusInfo->pOutBuffLast[i] = NULL;
				} /* end if */
			} /* end for */
		}
	}
	else {
		/* Set Error Factor */
		if(ret == false) {
			/* RCG3AFLDL4001ZDO P-00006 start */
			if ( FLAC__stream_decoder_get_error_state(&(pStaticStructure->pStreamDecoder)) == FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC ) {
				pStaticStructure->nErrorFactor = FLACD_ERR_LOST_SYNC;
			}
			else {	/* RCG3AFLDL4001ZDO P-00006 end */
				FLAC__stream_decoder_flush(&(pStaticStructure->pStreamDecoder));
				pStaticStructure->nErrorFactor = FLACD_ERR_SHORT_INPUT_DATA;

				/* Reset Input Buffer Infomation */
				pBuffStatusInfo->nInBuffUsedDataSize = frameStartUsedDataSize;
				pBuffStatusInfo->pInBuffLast = pBuffConfigInfo->pInBuffStart + frameStartUsedDataSize;
			} /* end if */
		}
		else {
			switch (FLAC__stream_decoder_get_error_state(&(pStaticStructure->pStreamDecoder))) {
				case FLAC__STREAM_DECODER_ERROR_STATUS_UNSUPPORTED_STREAM :
					pStaticStructure->nErrorFactor = FLACD_ERR_NOT_SUPPORTED_DATA;
					break;
				case FLAC__STREAM_DECODER_ERROR_STATUS_CHANGE_HEADER :
					pStaticStructure->nErrorFactor = FLACD_ERR_CHANGE_FRAME_HEADER;
					break;
				case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC : 
				case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER : 
				case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM : 
				default :
					pStaticStructure->nErrorFactor = FLACD_ERR_LOST_SYNC;
					break;
			} /* end switch */
		} /* end if */
		return FLACD_RESULT_NG;
	} /* end if */
	
	/* Set to Buffer Status Information Structure */
	return FLACD_RESULT_OK;
}
/*==============================================================================*/
/*
*
* FUNCTION NAME
* 		output16pcm
*
* ARGUMENT
* 		ACMW_INT16*			const pDstBuff
*		const FLAC__int32*	const pSrcBuff
*		const ACMW_UINT32	block_length
*		const int32_t		nShiftBits
*
* RETURN VALUE
* 		void
*
* OVERVIEW
* 		Output PCM Data(16bit)
*
* DIVERSION ORIGIN
* 		Original.
*
*/
static void output16pcm( ACMW_INT16*		const pDstBuff,
						 const FLAC__int32*	const pSrcBuff,
						 const ACMW_UINT32	block_length,
						 const ACMW_INT32	nShiftBits
						 )
{
	ACMW_UINT32	i;
	if (pSrcBuff == NULL) {
		for ( i = 0; i < block_length ; i++ ) {
		  pDstBuff[i] = 0;
		} /* end for */
	} else {
		if ( nShiftBits > (ACMW_INT32)0 ) {
			for ( i = 0; i < block_length ; i++ ) {
				pDstBuff[i] = (ACMW_INT16)( pSrcBuff[i] << nShiftBits );
			} /* end for */
		} else if (nShiftBits < (ACMW_INT32)0) {
			for ( i = 0; i < block_length ; i++ ) {
				pDstBuff[i] = (ACMW_INT16)( pSrcBuff[i] >> (-nShiftBits) );
			} /* end for */
		} else {
			for ( i = 0; i < block_length ; i++ ) {
				pDstBuff[i] = (ACMW_INT16)pSrcBuff[i];
			} /* end for */
		} /* end if */
	} /* end if */
}
/*------------------------------------------------------------------------------*/

/*==============================================================================*/
/*
*
* FUNCTION NAME
* 		output24pcm
*
* ARGUMENT
* 		ACMW_INT32*			const pDstBuff
*		const FLAC__int32*	const pSrcBuff
*		const ACMW_UINT32	block_length
*
* RETURN VALUE
* 		void
*
* OVERVIEW
* 		Output PCM Data(24bit)
*
* DIVERSION ORIGIN
* 		Original.
*
*/
static void output24pcm( ACMW_INT32*		const pDstBuff,
						 const FLAC__int32*	const pSrcBuff,
						 const ACMW_UINT32	block_length,
						 const ACMW_INT32	nShiftBits
						 )
{
	ACMW_UINT32	i;
	if (pSrcBuff == NULL) {
		for ( i = 0; i < block_length ; i++ ) {
		  pDstBuff[i] = 0;
		} /* end for */
	} else {
		for ( i = 0; i < block_length ; i++ ) {
			pDstBuff[i] = (ACMW_INT32)( pSrcBuff[i] << nShiftBits );
		} /* end for */
	} /* end if */
}
/*------------------------------------------------------------------------------*/
