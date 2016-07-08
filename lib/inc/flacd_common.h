/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/******************************************************************************
* File Name    : flacd_common.h
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description  : -
******************************************************************************/
#ifndef FLACD_COMMON_H
#define FLACD_COMMON_H

#include "flacd_Lib.h"

#define FLAC__NO_DLL

#ifdef RCAR_H3_M3
#define FLACD_ENABLE_SF_192KHZ
#endif	/* RCAR_H3_M3 */


/*==========================================================================*/
/*      Rename Define                                                       */
/*==========================================================================*/
#define FLAC_API_SUPPORTS_OGG_FLAC	flacd_const_FLAC_API_SUPPORTS_OGG_FLAC
#define FLAC__crc8_table			flacd_const_FLAC__crc8_table
#define FLAC__crc16_table			flacd_const_FLAC__crc16_table
#define FLAC__FRAME_FOOTER_CRC_LEN	flacd_const_FLAC__FRAME_FOOTER_CRC_LEN
#define FLAC__ENTROPY_CODING_METHOD_TYPE_LEN						flacd_const_FLAC__ENTROPY_CODING_METHOD_TYPE_LEN
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN		flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ORDER_LEN
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN	flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN	flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_RAW_LEN		flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_RAW_LEN
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER	flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER
#define FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_ESCAPE_PARAMETER	flacd_const_FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_ESCAPE_PARAMETER
#define FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN	flacd_const_FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN
#define FLAC__SUBFRAME_LPC_QLP_SHIFT_LEN			flacd_const_FLAC__SUBFRAME_LPC_QLP_SHIFT_LEN

#define FLAC__bitreader_new							flacd_bitreader_new
#define FLAC__bitreader_init						flacd_bitreader_init
#define FLAC__bitreader_reset_read_crc16			flacd_bitreader_reset_read_crc16
#define FLAC__bitreader_get_read_crc16				flacd_bitreader_get_read_crc16
#define FLAC__bitreader_read_raw_uint32				flacd_bitreader_read_raw_uint32
#define FLAC__bitreader_read_raw_int32				flacd_bitreader_read_raw_int32
#define FLAC__bitreader_read_unary_unsigned			flacd_bitreader_read_unary_unsigned
#define FLAC__bitreader_read_rice_signed_block		flacd_bitreader_read_rice_signed_block
#define FLAC__bitreader_read_utf8_uint32			flacd_bitreader_read_utf8_uint32
#define FLAC__bitreader_read_utf8_uint64			flacd_bitreader_read_utf8_uint64
#define FLAC__stream_decoder_new					flacd_stream_decoder_new
#define FLAC__stream_decoder_flush					flacd_stream_decoder_flush
#define FLAC__stream_decoder_reset					flacd_stream_decoder_reset
#define FLAC__stream_decoder_init_stream			flacd_stream_decoder_init_stream
#define FLAC__stream_decoder_get_state				flacd_stream_decoder_get_state
#define FLAC__stream_decoder_get_channels			flacd_stream_decoder_get_channels
#define FLAC__stream_decoder_get_bits_per_sample	flacd_stream_decoder_get_bits_per_sample
#define FLAC__stream_decoder_get_sample_rate		flacd_stream_decoder_get_sample_rate
#define FLAC__stream_decoder_get_blocksize			flacd_stream_decoder_get_blocksize
#define FLAC__stream_decoder_set_supported_channels	flacd_stream_decoder_set_supported_channels
#define FLAC__stream_decoder_set_streaminfo			flacd_stream_decoder_set_streaminfo
#define FLAC__stream_decoder_get_error_state		flacd_stream_decoder_get_error_state
#define FLAC__stream_decoder_allocate_residual		flacd_stream_decoder_allocate_residual
#define FLAC__stream_decoder_allocate_output		flacd_stream_decoder_allocate_output
#define FLAC__stream_decoder_process_frame_sync		flacd_stream_decoder_process_frame_sync
#define FLAC__stream_decoder_process_read_frame		flacd_stream_decoder_process_read_frame
#define FLAC__crc8									flacd_crc8
#define FLAC__fixed_restore_signal					flacd_fixed_restore_signal
#define FLAC__lpc_restore_signal					flacd_lpc_restore_signal
#define FLAC__lpc_restore_signal_wide				flacd_lpc_restore_signal_wide

/*==========================================================================*/
/*      Define Type                                                         */
/*==========================================================================*/
#if !defined ( NULL )
    #define NULL  ( (void *) 0 )
#endif

/*==========================================================================*/
/*      Function Prototype                                                  */
/*==========================================================================*/
void* flacd_MemSet(void* const pTo, const ACMW_UINT8 nData, const ACMW_INT32 nSize);
void* flacd_MemCpy(void* const pDest, const void* const pSrc, const ACMW_INT32 nSize);
void* flacd_MemMove(void* const pDest, const void* const pSrc, const ACMW_INT32 nSize);
ACMW_INT32 flacd_MemCmp(void* const pSrc1, void* const pSrc2, const ACMW_INT32 nSize );

#endif	/* FLACD_COMMON_H */
/*==========================================================================*/
/*      End of File                                                         */
/*==========================================================================*/
