/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_GetMemorySize.c
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description : Source file for API
******************************************************************************/

#include "flacd_Lib.h"
#include "flacd_api.h"
#include "flacd_common.h"

/*==========================================================================
 [Function Name]
   flacd_GetMemorySize

 [Description]
   Get Memory Size of the FLACD SW.IP. This is a top level wrapper.

 [Prototype]
   INT32  flacd_GetMemorySize(
           flacd_getMemorySizeConfigInfo *pGetMemorySizeConfigInfo,
           flacd_getMemorySizeStatusInfo *pGetMemorySizeStatusInfo)

 [Argument]
   const flacd_getMemorySizeConfigInfo *  pGetMemorySizeConfigInfo
           Pointer to Get Memory Size Configuration Information Structure
   const flacd_getMemorySizeStatusInfo *  pGetMemorySizeStatusInfo
           Pointer to Get Memory Size Status Information Structure

 [Return]
   INT32 Error Code
           FLACD_RESULT_OK       : Normal
           FLACD_RESULT_FATAL    : Error (none Error Factor)
                                 : static region could not be established.

 [Note]
   None
==========================================================================*/
ACMW_INT32	flacd_GetMemorySize(const flacd_getMemorySizeConfigInfo* const pGetMemorySizeConfigInfo,
								flacd_getMemorySizeStatusInfo* const pGetMemorySizeStatusInfo)
{
	ACMW_UINT32 nInputChan, nOutputChan;

	/* Argument Check */
	if (pGetMemorySizeConfigInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		return FLACD_RESULT_FATAL;
	} /* end if */

	if (pGetMemorySizeStatusInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		return FLACD_RESULT_FATAL;
	} /* end if */
	
	switch ((ACMW_INT32)pGetMemorySizeConfigInfo->nInputChannel) {
		case FLACD_INITCFG_2CHIN :
			nInputChan = FLACD_2CHIN;
			break;
		default :
			nInputChan = FLACD_6CHIN;
			break;
	} /* end switch */
	switch ((ACMW_INT32)pGetMemorySizeConfigInfo->nOutputChannel) {
		case FLACD_INITCFG_2CHOUT :
			nOutputChan = FLACD_2CHOUT;
			break;
		case FLACD_INITCFG_3CHOUT :
			nOutputChan = FLACD_3CHOUT;
			break;
		case FLACD_INITCFG_6CHOUT :
			nOutputChan = FLACD_6CHOUT;
			break;
		default :
			return FLACD_RESULT_FATAL;
	} /* end switch */

	/* Set to Memory Size Status Information Structure */
	pGetMemorySizeStatusInfo->nStaticSize = sizeof(flacd_StaticArea);
	pGetMemorySizeStatusInfo->nScratchSize = (ACMW_UINT32)(sizeof(flacd_ScratchArea) * nInputChan);
	pGetMemorySizeStatusInfo->nInputBufferSize = (FLACD_INBUFF_DATA_SIZE * nInputChan) + FLACD_INBUFF_HEADER_SIZE;
	if ( pGetMemorySizeConfigInfo->nOutBitsPerSample == 0x0000u ) {
		pGetMemorySizeStatusInfo->nOutputBufferSize = FLAC__SPEC_MAX_BLOCK_SIZE * sizeof(ACMW_INT16);	/* RCG3AFLDL4001ZDO P-00016 */
	} else {
		pGetMemorySizeStatusInfo->nOutputBufferSize = FLAC__SPEC_MAX_BLOCK_SIZE * sizeof(ACMW_INT32);	/* RCG3AFLDL4001ZDO P-00016 */
	}
	pGetMemorySizeStatusInfo->nStackSize = FLACD_STACK_SIZE;
	
	return FLACD_RESULT_OK;
}
