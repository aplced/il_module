/* ==========================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file comm_lib_standalone.c
*
* Communication Layer Main Functions (UART comminication)
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
*! 13-07-2010: created.
*!
* =========================================================================== */

/* User code goes here */
/* ------compilation control switches --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#if 0
#define SCREXEC_DBG_PRINT(ARGS...) MMS_IL_PRINT(ARGS)
#else
#define SCREXEC_DBG_PRINT(ARGS...)
#endif
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/* ========================================================================== */
/**
* @fn readMsg() TODO:
*
*  @see
*/
/* ========================================================================== */
static void readMsg(TIMM_OSAL_PTR pBuffer, TIMM_OSAL_U32 nNumBytes)
{
	TIMM_OSAL_U32 actSizeRead = 0;
	static systemMessage_t* tmpMsg = NULL;

	if (!pBuffer){
		SCREXEC_DBG_PRINT("Invalid return buffer supplied!\n");
		return TIMM_OSAL_ERR_PARAMETER;
	}

	if(tmpMsg == NULL){
		if (TIMM_OSAL_ERR_NONE != TIMM_OSAL_ReadFromPipe(STAND1_Script_Execution_PipeO, &tmpMsg, sizeof(systemMessage_t*), &actSizeRead, TIMM_OSAL_SUSPEND)) {
			SCREXEC_DBG_PRINT("TIMM_OSAL_ReadFromPipe failed!\n");
			return TIMM_OSAL_ERR_UNKNOWN;
		}
	}

	if(actSizeRead != sizeof(systemMessage_t*)){
		STAND1_DBG_PRINT("Requested %d bytes but read %d bytes\n", nNumBytes, actSizeRead);
		return TIMM_OSAL_ERR_UNKNOWN;
	}

	if(tmpMsg == NULL){
		SCREXEC_DBG_PRINT("Non-existent message!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

	TIMM_OSAL_Memcpy(pBuffer, tmpMsg, sizeof(systemMessage_t));

	if(tmpMsg->nPayloadSize == 0){
		SCREXEC_DBG_PRINT("Successful message read!\nRead %08X", &tmpMsg);
		TIMM_OSAL_Free(tmpMsg);
		tmpMsg = NULL;
	}else {
		((systemMessage_t*)pBuffer)->pPayload = TIMM_OSAL_MallocExtn(tmpMsg->nPayloadSize, TIMM_OSAL_TRUE, 32, 0, NULL);
		TIMM_OSAL_Memcpy(((systemMessage_t*)pBuffer)->pPayload, tmpMsg->pPayload, tmpMsg->nPayloadSize);

		SCREXEC_DBG_PRINT("Successful message read!\nRead %08X", &tmpMsg);
		TIMM_OSAL_Free(tmpMsg->pPayload);
		TIMM_OSAL_Free(tmpMsg);
		tmpMsg = NULL;
	}

	return TIMM_OSAL_ERR_NONE;
}

/* ========================================================================== */
/**
* @fn writeMsg() TODO:
*
*  @see
*/
/* ========================================================================== */
static void writeMsg(TIMM_OSAL_PTR pBuffer, TIMM_OSAL_U32 nNumBytes)
{
	static systemMessage_t* tmpMsg = NULL;

	if (!pBuffer){
		SCREXEC_DBG_PRINT("Invalid return buffer supplied!\n");
		return TIMM_OSAL_ERR_PARAMETER;
	}

	if(nNumBytes == sizeof(systemMessage_t*)){
		tmpMsg = TIMM_OSAL_MallocExtn(sizeof(systemMessage_t), TIMM_OSAL_TRUE, 32, 0, NULL);
		TIMM_OSAL_Memcpy(tmpMsg, pBuffer, sizeof(systemMessage_t));
		if(tmpMsg->nPayloadSize > 0){
			tmpMsg->pPayload = TIMM_OSAL_MallocExtn(tmpMsg->nPayloadSize, TIMM_OSAL_TRUE, 32, 0, NULL);
			TIMM_OSAL_Memcpy(tmpMsg->pPayload, ((systemMessage_t*)pBuffer)->pPayload, tmpMsg->nPayloadSize);
		}else {
			tmpMsg->pPayload = NULL;
		}
		if (TIMM_OSAL_ERR_NONE != TIMM_OSAL_WriteToPipe(STAND1_Script_Execution_PipeI, &tmpMsg, sizeof(systemMessage_t*), TIMM_OSAL_SUSPEND)){
			SCREXEC_DBG_PRINT("TIMM_OSAL_WriteToPipe failed!\n");
			return TIMM_OSAL_ERR_UNKNOWN;
		}
		SCREXEC_DBG_PRINT("Successful message write!\nSent %08X", &tmpMsg);
	}else {
		SCREXEC_DBG_PRINT("Out of sequence message!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

	return TIMM_OSAL_ERR_NONE;
}
#include "func/scrMod_InitMessages.c"
#include "func/scrMod_01_Initial_OMX_set_up.c"
#include "func/scrMod_02_Start_preview.c"
#include "func/scrMod_02_Attach_JPEG_file_saver.c"
#include "func/scrMod_10_Enable_VideoOutPort_Bayer10.c"
#include "func/scrMod_04_Capture_start.c"
#include "func/scrMod_03_Stop_preview.c"
#include "func/scrMod_02_Deinit_OMX_camera.c"
/* ========================================================================== */
/**
* @fn Script_Execution_Thread() TODO:
*
*  @see
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE Script_Execution_Thread(TIMM_OSAL_U32 nArgc, TIMM_OSAL_PTR pArgv)
{
	SCREXEC_DBG_PRINT("Starting thread with %d aruments\n", nArgc);
	FILE* kbd = fopen("/dev/tty0", "r");

	scrMod_InitMessages();
	while(TIMM_OSAL_TRUE){
		TIMM_OSAL_U8 keypress;

		SCREXEC_DBG_PRINT("\nWaiting for user input...\n\n");

		fscanf(kbd, "%c", &keypress);

		if(keypress == 'q'){
			scrMod_01_Initial_OMX_set_up_exec();
		}
		if(keypress == 'w'){
			scrMod_02_Start_preview_exec();
		}
		if(keypress == 'e'){
			scrMod_02_Attach_JPEG_file_saver_exec();
		}
		if(keypress == 'r'){
			scrMod_10_Enable_VideoOutPort_Bayer10_exec();
		}
		if(keypress == 't'){
			scrMod_04_Capture_start_exec();
		}
		if(keypress == 'y'){
			scrMod_03_Stop_preview_exec();
		}
		if(keypress == 'a'){
			scrMod_02_Deinit_OMX_camera_exec();
		}
	}
}
