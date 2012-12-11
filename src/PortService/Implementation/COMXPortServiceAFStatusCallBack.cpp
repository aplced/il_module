#include "inc/PortService/Implementation/COMXPortServiceAFStatusCallBack.h"

TIMM_OSAL_ERRORTYPE COMXPortServiceAFStatusCallBack::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
        MMS_IL_PRINT("Failed to init ExtractMetada service\n");
		return err;
	}
    lastAFStatus = AF_STATUS_IDLE;
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceAFStatusCallBack::ConfigPortService()
{
   // MMS_IL_PRINT("Reconfiguring port service AFStatusCallBack!\n");
    //int err = TIMM_OSAL_ERR_NONE;
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceAFStatusCallBack::Deinit()
{
    return this->COMXPortService::Deinit();
}


TIMM_OSAL_ERRORTYPE COMXPortServiceAFStatusCallBack::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
	int err = 0;
    struct AF_Callback_t AF_Callback;
    systemMessage_t message;

    eAllocType = eAllocType; //stop warning: unused parameter

	if(!StartExecute())
	{
        MMS_IL_PRINT("\nAFStatusCallBack: service not STARTED!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    OMX_OTHER_EXTRADATATYPE *extraData = pComp->getExtradata(pBufHdr->pPlatformPrivate, (OMX_EXTRADATATYPE)OMX_AncillaryData);
    if (extraData == NULL)
    {
        StopExecute();
        return TIMM_OSAL_ERR_MSG_TYPE_NOT_FOUND;
    }
    //MMS_IL_PRINT("extraData %p!\n", extraData);
    FOCUS_STATUS_VALUES AFstat =  (FOCUS_STATUS_VALUES)((OMX_TI_ANCILLARYDATATYPE*)extraData->data)->nAFStatus;
    //MMS_IL_PRINT("AFstat %d!\n", AFstat);
    if (lastAFStatus == AF_STATUS_RUNNING || lastAFStatus == AF_STATUS_IDLE)
    {
        if ((lastAFStatus == AF_STATUS_RUNNING && AFstat != AF_STATUS_RUNNING)
             || (lastAFStatus == AF_STATUS_IDLE && AFstat != AF_STATUS_IDLE)
            ||(lastAFStatus == AF_STATUS_SUSPEND && AFstat == AF_STATUS_SUSPEND))
        {
            extraData = pComp->getExtradata(pBufHdr->pPlatformPrivate, (OMX_EXTRADATATYPE)OMX_FocusRegion);
            if (extraData)
            {
                OMX_FOCUSREGIONTYPE* data = (OMX_FOCUSREGIONTYPE*)extraData->data;
                AF_Callback.nLeft = data->nLeft;
                AF_Callback.nTop = data->nTop;
                AF_Callback.nWidth = data->nWidth;
                AF_Callback.nHeight = data->nHeight;
            } else
            {
                MMS_IL_PRINT("No OMX_FocusRegion data is found!\n");
                AF_Callback.nLeft = 0;
                AF_Callback.nTop = 0;
                AF_Callback.nWidth = 0;
                AF_Callback.nHeight = 0;
            }

            MMS_IL_PRINT("AFStatusCallBack: Staning completed!\n");
            message.nId = OMX_AF_CAF_DONE;
            message.nStatus = 0;
            message.nPayloadSize = sizeof(struct AF_Callback_t);

            AF_Callback.AFstat = AFstat;
            message.pPayload = &AF_Callback;
            pComp->dispatcher->SendMessage(&message);
        }
    }
    lastAFStatus = AFstat;

    StopExecute();
    return err;
}
