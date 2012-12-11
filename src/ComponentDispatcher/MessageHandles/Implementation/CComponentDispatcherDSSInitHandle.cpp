#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSInitHandle.h"
#include "inc/Display/CDisplay.h"
#include "inc/Display/CDisplayFactory.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDSSInitHandle::Process(void* pMessage)
{
    COMXComponent* compHdl;
	COMXPortHandle* portData;
    //unsigned int nPortIdx;
    displaySetUp_t* displaySetUp;

    displaySetUp = (displaySetUp_t*)((systemMessage_t *)pMessage)->pPayload;
    int payloadsize = ((systemMessage_t *)pMessage)->nPayloadSize;
    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(displaySetUp->nComponentId);

    if (NULL == compHdl) {
		MMS_IL_PRINT("Unknown component %d\n", displaySetUp->nComponentId);
        return TIMM_OSAL_ERR_OMX;
    }

	portData = compHdl->GetPortData(displaySetUp->nPortNumber);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d port data\n", displaySetUp->nPortNumber);
		return TIMM_OSAL_ERR_OMX;
	}

    //Set Input and Output Port
    if (portData->tPortDef.eDir != OMX_DirOutput)
	{
		MMS_IL_PRINT("Requested port: %d is not an output port\n", displaySetUp->nPortNumber);
        return TIMM_OSAL_ERR_OMX;
    }

	if(compHdl->displayModule != NULL)
	{
		MMS_IL_PRINT("Deiniting previous display module\n");
		compHdl->displayModule->Deinit();
		free(compHdl->displayModule);
	}
    //Creates new object of type DSS_DisplayType or SurfaceFlinger_DisplayType
	compHdl->displayModule = CDisplayFactory::CreateDisplayHandle(DSS_DisplayType);

	if(compHdl->displayModule == NULL)
	{
		MMS_IL_PRINT("Cannot create DSS display module\n");
        return TIMM_OSAL_ERR_ALLOC;
	}
	//Opens Display and inits composition

    portData->nDisplayId = OMAP_DSS_CHANNEL_LCD;
    MMS_IL_PRINT("Initing Display!\n");
    if(payloadsize > 3 *sizeof(TIMM_OSAL_U32))
    {
            return compHdl->displayModule->Init(&displaySetUp->Config);
    } else
        return compHdl->displayModule->Init(NULL);

    return TIMM_OSAL_ERR_NONE;
}