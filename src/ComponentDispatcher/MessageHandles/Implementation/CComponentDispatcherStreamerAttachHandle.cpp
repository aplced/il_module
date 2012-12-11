#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherStreamerAttachHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherStreamerAttachHandle::Process(void* pMessage)
{
    COMXComponent* compHdl;
    COMXPortHandle* portData;
    unsigned int nPortIdx;
    dataTransferSetUp_t* strmSetUp;
    dataTransferDescT* tSysStrm = NULL;

    unsigned int bEnable = 0;

    MMS_IL_PRINT("Stream service start up\n");

    strmSetUp = (dataTransferSetUp_t *)((systemMessage_t *)pMessage)->pPayload;

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(strmSetUp->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	portData = compHdl->GetPortData(strmSetUp->nPortNumber);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data\n",  strmSetUp->nPortNumber);
		return TIMM_OSAL_ERR_OMX;
	}

    if (portData->tPortDef.eDir != OMX_DirOutput)
	{
        return TIMM_OSAL_ERR_OMX;
    }

    if (((systemMessage_t *)pMessage)->nStatus == MSG_STATUS_ENABLE)
	{
        bEnable = 1;
        //Allocate system stream
        tSysStrm = (dataTransferDescT*)malloc(sizeof(dataTransferDescT));
        if (tSysStrm == NULL)
		{
            MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
            return TIMM_OSAL_ERR_ALLOC;
        }
        TIMM_OSAL_Memset(tSysStrm, 0, sizeof(dataTransferDescT));
        //Set tream params
        tSysStrm->initiatorID = strmSetUp->nComponentId;
        tSysStrm->initiatorPort = strmSetUp->nPortNumber;
        tSysStrm->dataBuffPtr = NULL;
        tSysStrm->dataSize = 0;
        tSysStrm->dataStride = 0;
        tSysStrm->dataTypeID = strmSetUp->nDataType;
        tSysStrm->transferMsgIdD = strmSetUp->nMsgId;
        if (strmSetUp->nTransportType == 0)
		{
            tSysStrm->tranfMedia = DTM_MSG;
        } 
		else if (strmSetUp->nTransportType == 1)
		{
            tSysStrm->tranfMedia = DTM_FILE_IO;
            if (strlen(strmSetUp->cFileExt) > 7)
			{
                MMS_IL_PRINT("Maximum file extension length allowed is 7\n");
                free(tSysStrm);
                return TIMM_OSAL_ERR_UNKNOWN;
            }
            strcpy((char *) tSysStrm->fileExt, strmSetUp->cFileExt);
        } 
		else
		{
            tSysStrm->tranfMedia = DTM_UNDEF;
        }
        tSysStrm->customData = compHdl;
        tSysStrm->OnTransferDone = StreamPortDataDoneCallback;
    }

	if (portData->bSysStrm != NULL)
	{
		free(portData->bSysStrm);
		portData->bSysStrm = NULL;
	}
	if (bEnable)
	{   //Attach stream to the given component port
		portData->bSysStrm = tSysStrm;
	}

    return TIMM_OSAL_ERR_NONE;
}