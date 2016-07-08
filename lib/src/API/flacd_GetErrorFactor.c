/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_GetErrorFactor.c
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description : Source file for API
******************************************************************************/

#include "flacd_Lib.h"
#include "flacd_api.h"
#include "flacd_common.h"

/*==========================================================================
 [Function Name]
   flacd_GetErrorFactor

 [Description]
   Get Error Factor of the FLACD SW.IP. This is a top level wrapper.

 [Prototype]
   UINT32	flacd_GetErrorFactor(
             flacd_workMemoryInfo *pWorkMemInfo)

 [Argument]
   const flacd_workMemoryInfo *    pWorkMemInfo
           Pointer to Work Memory Information Structure

 [Return]
   UINT32   ErrorFactor
             FLACD_RESULT_FATAL            : Error (none Error Factor)
                                           : static region could not be established.
             FLACD_ERR_NONE                : Normal
             FLACD_ERR_POINTER             : Pointer is incorrect.
             FLACD_ERR_PARAMETER           : Parameter is incorrect.
             FLACD_ERR_SEQUENCE            : Sequence is incorrect.
             FLACD_ERR_SHORT_INPUT_DATA    : An error of input data shortage has occurred.
             FLACD_ERR_NOT_SUPPORTED_DATA  : It isn't supported.
             FLACD_ERR_LOST_SYNC           : Syncword isn't found.
             FLACD_ERR_CHANGE_FRAME_HEADER : Sampling Frequency is changed.
             FLACD_ERR_CRC                 : CRC error occurrs.
             FLACD_ERR_DECODE              : An error occurrs during decoding.

 [Note]
   None
==========================================================================*/
ACMW_UINT32	flacd_GetErrorFactor(const flacd_workMemoryInfo* const pWorkMemInfo)
{
	flacd_StaticArea *pStaticStructure;
	const ACMW_INT32 ret = FLACD_RESULT_FATAL;
	

	/* Argument Check */
	if (pWorkMemInfo == NULL) {	/* [2015.09.04] -> Alignment do not check for structure pointer */
		return (ACMW_UINT32)ret;
	} /* end if */

	if ((pWorkMemInfo->pStatic == NULL) || ((((ACMW_UINT32)(pWorkMemInfo->pStatic)) & FLACD_ALIGNMENT_PTR) != (ACMW_UINT32)0U)) {
		return (ACMW_UINT32)ret;
	} /* end if */
	
	pStaticStructure = (flacd_StaticArea*)pWorkMemInfo->pStatic;

	if (pStaticStructure->nEnableErrorFactor != FLACD_ERROR_FACTOR_ENABLE) {
		return (FLACD_ERR_SEQUENCE);
	} /* end if */

	/* Set to Return Code */
	return pStaticStructure->nErrorFactor;
}
