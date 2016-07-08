/*------------------------------------------------------------------------------*/
/* FLAC Decode Software                                                         */
/* Copyright(C) 2012-2016 Renesas Electronics Corporation.                      */
/*------------------------------------------------------------------------------*/

/*******************************************************************************
* File Name   : flacd_GetVersion.c
* $Rev: 2083 $
* $Date:: 2014-05-09 08:52:44 +0900#$
* Description : Source file for API
******************************************************************************/

#include "flacd_Lib.h"
#include "flacd_api.h"
#include "flacd_common.h"

const ACMW_UINT8 flacd_const_idstrings[FLACD_IDSTRINGS_LENGTH] = FLACD_ID_STRINGS;

/*==========================================================================
 [Function Name]
   flacd_GetVersion

 [Description]
   Get Version Information of the FLACD SW.IP. This is a top level wrapper.

 [Proto Type]
   UINT32  flacd_GetVersion(VOID)

 [Argument]
   None

 [Return]
   UINT32 Version Information
           +----+----+----+----+
           |8bit|8bit|8bit|8bit|
           +----+----+----+----+
             P1   P2   P3   P4

           P1 : Customer ID (0x00 : default)
           P2 : Release ID  (Release Version : 0x00)
              :             (alpha version   : 0xA* (*:0-9))
              :             (beta version    : 0xB* (*:0-9))
           P3 : Major version information
           P4 : Minor version information

           ex) Version 1.23, Release version
               0x00000123

 [Note]
   None
==========================================================================*/
ACMW_UINT32	flacd_GetVersion( void )
{
	return ( FLACD_CUSTOMERID << FLACD_SHIFT_24 ) | ( FLACD_RELEASEID << FLACD_SHIFT_16 ) | ( FLACD_MAJOR << FLACD_SHIFT_08 ) | FLACD_MINOR;
}
