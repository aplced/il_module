#include "inc/PortService/Implementation/COMXPortServiceExtractMetad.h"

TIMM_OSAL_ERRORTYPE COMXPortServiceExtractMetad::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
        MMS_IL_PRINT("Failed to init ExtractMetada service\n");
		return err;
	}

   if (pServiceHeader != NULL)
    {
         char* ptr = (char*)pServiceHeader;
         strcpy(ServiceHeaderSaveBuffer.namePrfx, ptr);
         int len = strlen(ServiceHeaderSaveBuffer.namePrfx);
         ptr += len;
         while (strcmp(ptr, "\0") == 0)
             ptr++;
         strcpy(ServiceHeaderSaveBuffer.fileExt, ptr);
    }else
    {
         strcpy(ServiceHeaderSaveBuffer.namePrfx, "\0");
         strcpy(ServiceHeaderSaveBuffer.fileExt, "\0");
    }
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceExtractMetad::ConfigPortService()
{

    MMS_IL_PRINT("Reconfiguring port service Extract Metada!\n");
    int err = TIMM_OSAL_ERR_NONE;
    if (pConfigData != NULL)
    {
        if (StartExecute())
        {
            //Init(pComp, associatedPort, NULL);
            memset(ServiceHeaderSaveBuffer.namePrfx, '\0', strlen(ServiceHeaderSaveBuffer.namePrfx));
            char* ptr = (char*)pConfigData;
            strcpy(ServiceHeaderSaveBuffer.namePrfx, ptr);
            MMS_IL_PRINT("new namePrfx - %s, new fileExt - %s!\n", ServiceHeaderSaveBuffer.namePrfx, ServiceHeaderSaveBuffer.fileExt);
            if (strcmp(ServiceHeaderSaveBuffer.namePrfx, ptr))
            {
                MMS_IL_PRINT("Setting name prefix failed!\n");
                err =  TIMM_OSAL_ERR_UNKNOWN;
            } else MMS_IL_PRINT("Service is reconfigured!\n");
            StopExecute();
        } else  MMS_IL_PRINT("Service is not enabled or config data is NULL!\n");
    }
    return err;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceExtractMetad::Deinit()
{
    return this->COMXPortService::Deinit();
}


TIMM_OSAL_ERRORTYPE COMXPortServiceExtractMetad::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
	int err = 0;

    CTransferBufferSave2File SaveBuffer;

     eAllocType = eAllocType; //stop warning: unused parameter

	if(!StartExecute())
	{
        printf("\nExtractMetada: service not STARTED!");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    OMX_TI_PLATFORMPRIVATE* pvtData = (OMX_TI_PLATFORMPRIVATE*)(pBufHdr->pPlatformPrivate);

    if(pvtData->nMetaDataSize > 0 && pvtData->pMetaDataBuffer != NULL)
    {
        SaveBuffer.Init(pComp,  associatedPort, pvtData->nMetaDataSize, &ServiceHeaderSaveBuffer);
        SaveBuffer.Transfer(pvtData->pMetaDataBuffer);

    }

    StopExecute();
    return err;
}
