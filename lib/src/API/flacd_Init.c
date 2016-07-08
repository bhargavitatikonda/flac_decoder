/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_Init.c
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description : Source file for API
******************************************************************************/

#include "flacd_common.h"
#include "FLAC/stream_decoder.h"
#include "flacd_Lib.h"
#include "flacd_api.h"

/*==========================================================================
 [Function Name]
   flacd_Init

 [Description]
   Initializes the FLAC SW.IP. This is a top level wrapper.

 [Prototype]
   INT32  flacd_Init(
           flacd_workMemInfo *pWorkMemInfo,
           flacd_initConfigInfo *pInitConfigInfo)

 [Argument]
   const flacd_workMemoryInfo *       pWorkMemInfo
           Pointer to Work Memory Information Structure
   const flacd_initConfigInfo *       pInitConfigInfo
           Pointer to Initialize Configuration Information Structure

 [Return]
   INT32 Error Code
           FLACD_RESULT_OK      : Normal
           FLACD_RESULT_NG      : Error (+ Error Factor)
           FLACD_RESULT_FATAL   : Error (none Error Factor)#include "FLAC/stream_decoder.h"


 [Note]
   None
==========================================================================*/
ACMW_INT32	flacd_Init( const flacd_workMemoryInfo* const pWorkMemInfo,
						const flacd_initConfigInfo* const pInitConfigInfo )
{
	flacd_StaticArea *pStaticStructure;
	FLAC__StreamDecoder *decoder;
	
	/* Argument Check */
	if (pWorkMemInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		return FLACD_RESULT_FATAL;
	} /* end if */
	
	if ((pWorkMemInfo->pStatic == NULL) || (((ACMW_UINT32)pWorkMemInfo->pStatic & FLACD_ALIGNMENT_PTR) != (ACMW_UINT32)0U)) {
		return FLACD_RESULT_FATAL;
	} /* end if */
	
	pStaticStructure = (flacd_StaticArea*)pWorkMemInfo->pStatic;
	
	/* Fill Static Work Memory */
	flacd_MemSet(pStaticStructure, 0, sizeof(flacd_StaticArea));
	
	/* Set ErrorCode */
	pStaticStructure->nErrorFactor = FLACD_ERR_NONE;
	pStaticStructure->nEnableErrorFactor = FLACD_ERROR_FACTOR_ENABLE;

	if ((pWorkMemInfo->pScratch == NULL) || ((((ACMW_UINT32)(pWorkMemInfo->pScratch)) & FLACD_ALIGNMENT_PTR) != (ACMW_UINT32)0U)) {
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	if (pInitConfigInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		pStaticStructure->nErrorFactor = FLACD_ERR_POINTER;
		return FLACD_RESULT_NG;
	} /* end if */
	
	/* Copy Config Value */
	switch ((ACMW_INT32)pInitConfigInfo->nInputChannel) {
		case FLACD_INITCFG_2CHIN :
			pStaticStructure->nInputChannel = (ACMW_UINT16)FLACD_2CHIN;
			break;
		default :
			pStaticStructure->nInputChannel = (ACMW_UINT16)FLACD_6CHIN;
			break;
	} /* end switch */
	switch ((ACMW_INT32)pInitConfigInfo->nOutputChannel) {
		case FLACD_INITCFG_2CHOUT :
			pStaticStructure->nOutputChannel = (ACMW_UINT16)FLACD_2CHOUT;
			break;
		case FLACD_INITCFG_3CHOUT :
			pStaticStructure->nOutputChannel = (ACMW_UINT16)FLACD_3CHOUT;
			break;
		case FLACD_INITCFG_6CHOUT :
			pStaticStructure->nOutputChannel = (ACMW_UINT16)FLACD_6CHOUT;
			break;
		default :
			pStaticStructure->nErrorFactor = FLACD_ERR_PARAMETER;
			return FLACD_RESULT_NG;
	} /* end switch */
	pStaticStructure->nOutBitsPerSample = pInitConfigInfo->nOutBitsPerSample;

	/* Fill Scratch Work Memory */
	flacd_MemSet(pWorkMemInfo->pScratch, 0, (ACMW_INT32)(sizeof(flacd_ScratchArea)*(ACMW_UINT32)pStaticStructure->nInputChannel));
	
	/* Initialize */
	decoder = FLAC__stream_decoder_new( pWorkMemInfo );
	FLAC__stream_decoder_init_stream( decoder );
	FLAC__stream_decoder_set_supported_channels( decoder, pStaticStructure->nInputChannel, pStaticStructure->nOutputChannel );
	
	return FLACD_RESULT_OK;
}
