/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_common.c
* $Rev: 1898 $
* $Date:: 2014-03-11 15:31:49 +0900#$
* Description : -
******************************************************************************/
#include "flacd_common.h"

#define GENERAL_MEMSIZE_MIN			((ACMW_INT32)0)
#define GENERAL_PROCESS_UNIT		((ACMW_INT32)4)
#define GENERAL_PROCESS_INDEX_0		((ACMW_INT32)0)
#define GENERAL_PROCESS_INDEX_1		((ACMW_INT32)1)
#define GENERAL_PROCESS_INDEX_2		((ACMW_INT32)2)
#define GENERAL_PROCESS_INDEX_3		((ACMW_INT32)3)

void* flacd_MemSet(void* const pTo, const ACMW_UINT8 nData, const ACMW_INT32 nSize)
{
	ACMW_INT32	index;
	ACMW_UINT8* const ptrDest = (ACMW_UINT8*)pTo;

	if ( ( ptrDest != NULL ) && ( GENERAL_MEMSIZE_MIN < nSize ) ) {
		/* The address of the setting destination isn't NULL and the established size is normal. */
		for ( index = 0; index < nSize; index++ ) {
			ptrDest[index] = nData;
		} /* end for */
	} else {
		/* The address of the setting destination is NULL or the established size is abnormal. */
		/* DO NOTHING */
	} /* end if */

	return (void*)ptrDest;
}

void* flacd_MemCpy(void* const pDest, const void* const pSrc, const ACMW_INT32 nSize)
{
	ACMW_INT32	index;
	const ACMW_UINT8* const ptrSrc = (const ACMW_UINT8*)pSrc;
	ACMW_UINT8* const ptrDest = (ACMW_UINT8*)pDest;

	if ( ( ptrDest != NULL) && ( ptrSrc != NULL) && ( ptrDest != ptrSrc ) && ( GENERAL_MEMSIZE_MIN < nSize ) ) {
		/* An argument index discords and, an argument index isn't NULL and the size is normal. */
		for ( index = 0; ( index + GENERAL_PROCESS_INDEX_3 ) < nSize; index += GENERAL_PROCESS_UNIT ) {
			ptrDest[index + GENERAL_PROCESS_INDEX_0] = ptrSrc[index + GENERAL_PROCESS_INDEX_0];
			ptrDest[index + GENERAL_PROCESS_INDEX_1] = ptrSrc[index + GENERAL_PROCESS_INDEX_1];
			ptrDest[index + GENERAL_PROCESS_INDEX_2] = ptrSrc[index + GENERAL_PROCESS_INDEX_2];
			ptrDest[index + GENERAL_PROCESS_INDEX_3] = ptrSrc[index + GENERAL_PROCESS_INDEX_3];
		} /* end for */
		for ( ; index < nSize; index ++ ) {
			ptrDest[index] = ptrSrc[index];
		} /* end for */
	} else {
		/* An argument index is identical and an argument index is NULL or the size is abnormal. */
		/* DO NOTHING */
	} /* end if */

	return (void*)ptrDest;
}

#if 0 /* not use */
void* flacd_MemMove(void* const pDest, const void* const pSrc, const ACMW_INT32 nSize)
{
	ACMW_INT32	index;
	const ACMW_UINT8* const ptrSrc = (const ACMW_UINT8*)pSrc;
	ACMW_UINT8* const ptrDest = (ACMW_UINT8*)pDest;

	if ( ( ptrDest != NULL) && ( ptrSrc != NULL) && ( ptrDest != ptrSrc ) && ( GENERAL_MEMSIZE_MIN < nSize ) ) {
		/* An argument index discords and, an argument index isn't NULL and the size is normal. */
		if ( ( ptrDest < ptrSrc ) && ( ptrSrc <= &ptrDest[nSize] ) ) {
			/* Movement origin is included in a movement destination. */
			for ( index = 0; index < nSize ; index++ ) {
				ptrDest[index] = ptrSrc[index];
			} /* end for */
		} else {
			/* Movement origin isn't included in a movement destination. */
			for ( index = 1; index <= nSize; index++ ) {
				ptrDest[nSize - index] = ptrSrc[nSize - index];
			} /* end for */
		} /* end if */
	} else {
		/* An argument index is identical and an argument index is NULL or the size is abnormal. */
		/* DO NOTHING */
	} /* end if */

	return (void*)ptrDest;
}

ACMW_INT32 flacd_MemCmp(void* const pSrc1, void* const pSrc2, const ACMW_INT32 nSize )
{
	ACMW_INT32	Ret = (ACMW_INT32)0;
	ACMW_INT32	index;
	ACMW_UINT32* const ptrSrc1 = (ACMW_UINT32*)pSrc1;
	ACMW_UINT32* const ptrSrc2 = (ACMW_UINT32*)pSrc2;

	if ( ( ptrSrc1 != NULL ) && ( ptrSrc2 != NULL ) && ( GENERAL_MEMSIZE_MIN < nSize ) ) {
		/* An argument index isn't NULL and the size is normal. */
		for (index = 0; index < nSize; index++ ) {
			if ( ( ACMW_UINT32 )ptrSrc1[index] != ( ACMW_UINT32 )ptrSrc2[index] ) {
				/* The value discords. */
				Ret = (ACMW_INT32)(ptrSrc1[index] - ptrSrc2[index]);
				break;
			} else {
				/* The value is identical. */
				/* DO NOTHIING */
			} /* end if */
		} /* end for */
	} else {
		/* An argument index is NULL or the size is abnormal. */
		/* DO NOTHING */
	} /* end if */

	return Ret;
}
#endif
/*==========================================================================*/
/* End of File                                                              */
/*==========================================================================*/
