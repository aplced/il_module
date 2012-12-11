/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file data_server.c
*
* OMX Message Handles
*
* @path WTSD_DucatiMMSW/framework/communication_layer/src/
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
*!
*! Revision History
*! ===================================
*! 20-04-2010 AAndreev: created.
*!
* =========================================================================== */

/* User code goes here */
/* ------compilation control switches --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/

#include "inc/lib_comm.h"
#include "inc/messages.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/

/*--------data declarations -----------------------------------*/
static CCommunicationChannel* pCommChan;
/*--------function prototypes ---------------------------------*/

/* ========================================================================== */
/**
* @fn copyFile2Buffer() Copies a file to a buffer
*
*  @see
*/
/* ========================================================================== */
TIMM_OSAL_U32 copyFile2Buffer(char* pFileName, TIMM_OSAL_PTR pDestBuff, TIMM_OSAL_U32 nCpySz)
{
    TIMM_OSAL_U32 readData = 0;
    FILE* fdBuffRead = fopen(pFileName, "rb");

    if (fdBuffRead)
    {
        printf("Opened file: %s\n", pFileName);
        readData = fread(pDestBuff, 1, nCpySz, fdBuffRead);
        fclose(fdBuffRead);
        printf("Read %d bytes from file %s to buffer 0x%08X\n", readData, pFileName, (TIMM_OSAL_U32)pDestBuff);
    } else
    {
        printf("Can not open file: %s\n", pFileName);
    }

    return readData;
}
