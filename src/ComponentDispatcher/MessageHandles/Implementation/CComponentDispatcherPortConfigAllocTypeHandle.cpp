#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortConfigAllocTypeHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortConfigAllocTypeHandle::Process(void* pMessage)
{
    COMXComponent* compHdl;
	COMXPortHandle* portData;

    omxPortAllocMethodSetUp_t* allocType;
    memAllocStrat_t allocStrat;

    allocType = (omxPortAllocMethodSetUp_t*)((systemMessage_t *)pMessage)->pPayload;

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(allocType->nComponentId);

    if (NULL == compHdl)
	{
		MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	portData = compHdl->GetPortData(allocType->nPortNumber);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data\n", allocType->nPortNumber);
		return TIMM_OSAL_ERR_OMX;
	}
    // Set Input and Output Port
 /*   if ((portData->tPortDef.eDir != OMX_DirOutput))
	{
        return TIMM_OSAL_ERR_UNKNOWN;
    }*/

    switch(allocType->nAllocMethod)
    {
        case 0:
            allocStrat = MEM_OMX;
            break;
        case 1:
            allocStrat = MEM_OSAL;
            break;
        case 2:
            allocStrat = MEM_ION_1D;
            break;
        case 3:
            allocStrat = MEM_ION_2D;
            break;
        default:
            allocStrat = MEM_UNDEF;
            break;
    }

	portData->eAllocType = allocStrat;

    return TIMM_OSAL_ERR_NONE;
}