#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetPortInfoHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetPortInfoHandle::Process(void* pMessage)
{
    int idx;
    unsigned int nPortCount = 0;
	int portInfoPayloadSize = 0;
    getportsMessageAck_t* pPortsList;
	COMXPortHandle* portData;

	unsigned int nComponentId = ((getportsMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nComponentId;

	COMXComponent* pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    nPortCount += pComp->VideoPortCount();
	nPortCount += pComp->AudioPortCount();
	nPortCount += pComp->ImagePortCount();
	nPortCount += pComp->OtherPortCount();

	portInfoPayloadSize = sizeof(getportsMessageAck_t) + nPortCount* sizeof(portData_t);
    //Allocate buffer for port list
    pPortsList = (getportsMessageAck_t*)malloc(portInfoPayloadSize);

    if (!pPortsList)
        return TIMM_OSAL_ERR_ALLOC;

    // Release the payload buffer since we do not need it anymore (we are going to use pPortsList as payload buffer from now on.)
    free(((systemMessage_t *)pMessage)->pPayload);

    // Update payload data to match the new payload
    ((systemMessage_t *)pMessage)->pPayload = pPortsList;
    ((systemMessage_t *)pMessage)->nPayloadSize = portInfoPayloadSize;

    nPortCount = 0;
    pPortsList->nComponentId = nComponentId;

    // Get data for component video buffers
    for (idx = pComp->VideoPortStartIndex(); idx < pComp->VideoPortCount(); idx++)
	{
		portData = pComp->GetPortData(idx);
		if(portData == NULL)
		{
			MMS_IL_PRINT("Failed to get port %d port data\n", idx);
			return TIMM_OSAL_ERR_OMX;
		}

        pPortsList->pPortData[nPortCount].nPortNo = portData->tPortDef.nPortIndex;
        pPortsList->pPortData[nPortCount].eDir = portData->tPortDef.eDir;
        pPortsList->pPortData[nPortCount++].eDomain = portData->tPortDef.eDomain;
    }

    // Get data for component audio buffers
    for (idx = pComp->AudioPortStartIndex(); idx < pComp->AudioPortCount(); idx++)
	{
		portData = pComp->GetPortData(idx);
		if(portData == NULL)
		{
			MMS_IL_PRINT("Failed to get port %d port data\n", idx);
			return TIMM_OSAL_ERR_OMX;
		}

        pPortsList->pPortData[nPortCount].nPortNo = portData->tPortDef.nPortIndex;
        pPortsList->pPortData[nPortCount].eDir = portData->tPortDef.eDir;
        pPortsList->pPortData[nPortCount++].eDomain = portData->tPortDef.eDomain;
    }

    // Get data for component image buffers
    for (idx = pComp->ImagePortStartIndex(); idx < pComp->ImagePortCount(); idx++)
	{
		portData = pComp->GetPortData(idx);
		if(portData == NULL)
		{
			MMS_IL_PRINT("Failed to get port %d port data\n", idx);
			return TIMM_OSAL_ERR_OMX;
		}

        pPortsList->pPortData[nPortCount].nPortNo = portData->tPortDef.nPortIndex;
        pPortsList->pPortData[nPortCount].eDir = portData->tPortDef.eDir;
        pPortsList->pPortData[nPortCount++].eDomain = portData->tPortDef.eDomain;
    }

    // Get data for component other buffers
    for (idx = pComp->OtherPortStartIndex(); idx < pComp->OtherPortCount(); idx++)
	{
		portData = pComp->GetPortData(idx);
		if(portData == NULL)
		{
			MMS_IL_PRINT("Failed to get port %d port data\n", idx);
			return TIMM_OSAL_ERR_OMX;
		}

        pPortsList->pPortData[nPortCount].nPortNo = portData->tPortDef.nPortIndex;
        pPortsList->pPortData[nPortCount].eDir = portData->tPortDef.eDir;
        pPortsList->pPortData[nPortCount++].eDomain = portData->tPortDef.eDomain;
    }

    return TIMM_OSAL_ERR_NONE;
}