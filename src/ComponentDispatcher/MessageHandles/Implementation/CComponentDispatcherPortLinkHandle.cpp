#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortLinkHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortLinkHandle::Process(void* pMessage)
{
	linkMessage_t* linkMsg;

    COMXComponent* Component1Handle;
	COMXPortHandle* portData1Comp;

	COMXComponent* Component2Handle;
	COMXPortHandle* portData2Comp;

    COMXComponent* ComponentOutputHandle;
	COMXPortHandle* pPortLinkOutput;

	COMXComponent* ComponentInputHandle;
	COMXPortHandle* pPortLinkInput;

    OMX_PARAM_PORTDEFINITIONTYPE* tPortCompInputDef;
	OMX_PARAM_PORTDEFINITIONTYPE* tPortCompOutputDef;

    //unsigned int nPortIdx;
    bool bOutPortIsSupplier = true;
    bool bInPortIsSupplier = false;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    MMS_IL_PRINT("Setting component port to port link\n");

	linkMsg = (linkMessage_t *)((systemMessage_t *)pMessage)->pPayload;

    Component1Handle = (COMXComponent*)dispatcher->GetOMXComponentSlot(linkMsg->nComponent1Id);
    if (NULL == Component1Handle)
	{
        MMS_IL_PRINT("First component %d is unknown\n", linkMsg->nComponent1Id);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    Component2Handle = (COMXComponent*)dispatcher->GetOMXComponentSlot(linkMsg->nComponent2Id);
    if (NULL == Component2Handle)
	{
        MMS_IL_PRINT("Second component %d is unknown\n", linkMsg->nComponent2Id);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	portData1Comp = Component1Handle->GetPortData(linkMsg->nComponent1Port);
	if(portData1Comp == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data from first component\n", linkMsg->nComponent1Port);
	}

	portData2Comp = Component2Handle->GetPortData(linkMsg->nComponent2Port);
	if(portData2Comp == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data from second component\n", linkMsg->nComponent2Port);
	}

    // The two ports should be from the same domain
  /*  if (portData1Comp->tPortDef.eDomain != portData2Comp->tPortDef.eDomain)
	{
        MMS_IL_PRINT("Port linking only possible between ports within the same domain!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }*/

    // Set Input and Output Port
    if ((portData1Comp->tPortDef.eDir == OMX_DirOutput) && (portData2Comp->tPortDef.eDir == OMX_DirInput))
	{
        ComponentInputHandle = Component2Handle;
        pPortLinkInput = portData2Comp;
        tPortCompInputDef = &(portData2Comp->tPortDef);

		ComponentOutputHandle = Component1Handle;
		pPortLinkOutput = portData1Comp;
        tPortCompOutputDef = &(portData1Comp->tPortDef);
    }
	else if ((portData2Comp->tPortDef.eDir == OMX_DirOutput) && (portData1Comp->tPortDef.eDir == OMX_DirInput))
	{
        ComponentInputHandle = Component1Handle;
		pPortLinkInput = portData1Comp;
        tPortCompInputDef = &(portData1Comp->tPortDef);

		ComponentOutputHandle = Component2Handle;
		pPortLinkOutput = portData2Comp;
        tPortCompOutputDef = &(portData2Comp->tPortDef);
    }
	else
	{
        MMS_IL_PRINT("Port linking only possible between source and sink port types!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (tPortCompOutputDef->nBufferCountActual != tPortCompInputDef->nBufferCountActual)
	{
        tPortCompOutputDef->nBufferCountActual = tPortCompInputDef->nBufferCountActual;

		eError = ComponentOutputHandle->SetParam(OMX_IndexParamPortDefinition, (unsigned char*)tPortCompOutputDef);
        if (eError != OMX_ErrorNone)
		{
            MMS_IL_PRINT("Exiting - setting new buffer count failed! Error - 0x%08x\n", eError);
            return TIMM_OSAL_ERR_OMX;
        }
        MMS_IL_PRINT("Successfuly set new buffer count to input port!\n");
    }

    pPortLinkOutput->pPortCallback = OutputPortCallback;
    pPortLinkInput->pPortCallback = InputPortCallback;

    pPortLinkInput->pLinkedPort = pPortLinkOutput;
    pPortLinkOutput->pLinkedPort = pPortLinkInput;

    pPortLinkOutput->pLinkedComponent = ComponentInputHandle;
    pPortLinkInput->pLinkedComponent = ComponentOutputHandle;

    pPortLinkOutput->bSupplierPort = bOutPortIsSupplier;
    pPortLinkInput->bSupplierPort = bInPortIsSupplier;

    MMS_IL_PRINT("Input link port address %p\n", pPortLinkInput);
    MMS_IL_PRINT("Output link port address %p\n", pPortLinkOutput);

    return TIMM_OSAL_ERR_NONE;
}