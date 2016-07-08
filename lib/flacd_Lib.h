/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

#ifndef FLACD_LIB_H
#define FLACD_LIB_H

/*==========================================================================*/
/*      Define Type for Software IP                                         */
/*==========================================================================*/
#ifndef ACMW_COMM_STDTYPES_H
#define ACMW_COMM_STDTYPES_H

typedef signed char						ACMW_INT8;
typedef signed short					ACMW_INT16;
typedef signed int						ACMW_INT32;
typedef unsigned char					ACMW_UINT8;
typedef unsigned short					ACMW_UINT16;
typedef unsigned int					ACMW_UINT32;
typedef signed short					ACMW_BOOL;

#endif	/* ACMW_COMM_STDTYPES_H */

/*==========================================================================*/
/*      Define Error Code                                                   */
/*==========================================================================*/
#define	FLACD_RESULT_OK					((ACMW_INT32)(0x00000000))
#define	FLACD_RESULT_NG					((ACMW_INT32)(0x00000001))
#define	FLACD_RESULT_WARNING			((ACMW_INT32)(0x00000002))
#define	FLACD_RESULT_FATAL				((ACMW_INT32)(0x00000003))


/*==========================================================================*/
/*      Define Error Factor                                                 */
/*==========================================================================*/
#define FLACD_ERR_NONE					((ACMW_UINT32)(0x00000000u))
#define FLACD_ERR_POINTER				((ACMW_UINT32)(0x00000010u))
#define FLACD_ERR_PARAMETER				((ACMW_UINT32)(0x00000020u))
#define FLACD_ERR_SEQUENCE				((ACMW_UINT32)(0x00000040u))
#define FLACD_ERR_SHORT_INPUT_DATA		((ACMW_UINT32)(0x00000100u))
#define FLACD_ERR_NOT_SUPPORTED_DATA	((ACMW_UINT32)(0x00001000u))
#define FLACD_ERR_LOST_SYNC				((ACMW_UINT32)(0x00010000u))
#define FLACD_ERR_CHANGE_FRAME_HEADER	((ACMW_UINT32)(0x00020000u))
#define FLACD_ERR_CRC					((ACMW_UINT32)(0x00200000u))
#define FLACD_ERR_DECODE				((ACMW_UINT32)(0x01000000u))


/*==========================================================================*/
/*      Define Struct flacd_getMemorySizeConfigInfo                        */
/*==========================================================================*/
/* [Description]                                                            */
/*   GetMemorySize Config Information Structure                             */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT16 nInputChannel;
	ACMW_UINT16 nOutputChannel;
	ACMW_UINT16 nOutBitsPerSample;
} flacd_getMemorySizeConfigInfo;


/*==========================================================================*/
/*      Define Struct flacd_getMemorySizeStatusInfo                        */
/*==========================================================================*/
/* [Description]                                                            */
/*   GetMemorySize Status Information Structure                             */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT32 nStaticSize;
	ACMW_UINT32 nScratchSize;
	ACMW_UINT32 nInputBufferSize;
	ACMW_UINT32 nOutputBufferSize;
	ACMW_UINT32 nStackSize;
} flacd_getMemorySizeStatusInfo;


/*==========================================================================*/
/*      Define Struct flacd_workMemInfo                                    */
/*==========================================================================*/
/* [Description]                                                            */
/*   Work Memory Information Structure                                      */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	void * pStatic;
	void * pScratch;
} flacd_workMemoryInfo;


/*==========================================================================*/
/*      Define Struct flacd_initConfigInfo                                 */
/*==========================================================================*/
/* [Description]                                                            */
/*   Initialize Config Information Structure                                */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT16 nInputChannel;
	ACMW_UINT16 nOutputChannel;
	ACMW_UINT16 nOutBitsPerSample;
} flacd_initConfigInfo;


/*==========================================================================*/
/*      Define Struct flacd_decConfigInfo                                  */
/*==========================================================================*/
/* [Description]                                                            */
/*   Decode Config Information Structure                                    */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT16 nMinBlockSize;
	ACMW_UINT16 nMaxBlockSize;
	ACMW_UINT32 nMinFrameSize;
	ACMW_UINT32 nMaxFrameSize;
	ACMW_UINT32 nSampleRate;
	ACMW_UINT16 nChannels;
	ACMW_UINT16 nBitsPerSample;
} flacd_decConfigInfo;


/*==========================================================================*/
/*      Define Struct flacd_decStatusInfo                                  */
/*==========================================================================*/
/* [Description]                                                            */
/*   Decode Status Information Structure                                    */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT32 nSampleRate;
	ACMW_UINT32 nDecodedSamples;
	ACMW_UINT16 nChannels;
	ACMW_UINT16 nChannelInfo;
	ACMW_UINT16 nBitsPerSample;
} flacd_decStatusInfo;


/*==========================================================================*/
/*      Define Struct flacd_ioBufferCoufigInfo                             */
/*==========================================================================*/
/* [Description]                                                            */
/*   Buffer Information Structure                                           */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT8	*pInBuffStart;
	ACMW_UINT32	nInBuffSetDataSize;
	void		**pOutBuffStart;
	ACMW_UINT32	nOutBuffSize;
} flacd_ioBufferConfigInfo;


/*==========================================================================*/
/*      Define Struct flacd_ioBufferStatusInfo                             */
/*==========================================================================*/
/* [Description]                                                            */
/*   Buffer Information Structure                                           */
/*                                                                          */
/* [Note]                                                                   */
/*   non                                                                    */
/*==========================================================================*/
typedef struct {
	ACMW_UINT8	*pInBuffLast;
	ACMW_UINT32	nInBuffUsedDataSize;
	void		**pOutBuffLast;
	ACMW_UINT32	nOutBuffUsedDataSize;
} flacd_ioBufferStatusInfo;


/*==========================================================================*/
/*      API Function Prototype                                              */
/*==========================================================================*/
extern ACMW_INT32 flacd_GetMemorySize(const flacd_getMemorySizeConfigInfo* const pGetMemorySizeConfigInfo,
                           flacd_getMemorySizeStatusInfo* const pGetMemorySizeStatusInfo);

extern ACMW_INT32 flacd_Init(const flacd_workMemoryInfo* const pWorkMemInfo,
                  const flacd_initConfigInfo* const pInitConfigInfo);

extern ACMW_INT32 flacd_Decode( const flacd_workMemoryInfo* const pWorkMemInfo,
                    const flacd_decConfigInfo* const pDecConfigInfo,
                    const flacd_ioBufferConfigInfo* const pBuffConfigInfo,
                    flacd_decStatusInfo* const pDecStatusInfo,
                    flacd_ioBufferStatusInfo* const pBuffStatusInfo);

extern ACMW_UINT32 flacd_GetErrorFactor(const flacd_workMemoryInfo* const pWorkMemInfo);

extern ACMW_UINT32 flacd_GetVersion(void);


#endif	/* FLACD_LIB_H */
/*==========================================================================*/
/*      End of File                                                         */
/*==========================================================================*/
