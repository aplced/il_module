#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocTypeHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemAllocTypeHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    marshalMemAllocMethodSetUp_t* marshalCtx = ((marshalMemAllocMethodSetUp_t *)((systemMessage_t *)pMessage)->pPayload);

    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    switch(marshalCtx->nAllocMethod)
    {
        case 1:
            pComp->tMemMarshallingAllocStrat = MEM_OSAL;
            break;
        case 2:
            pComp->tMemMarshallingAllocStrat = MEM_ION_1D;
            break;
        case 3:
            pComp->tMemMarshallingAllocStrat = MEM_ION_2D;
            break;
        default:
            pComp->tMemMarshallingAllocStrat = MEM_UNDEF;
            break;
    }

    if (pComp->tMemMarshallingAllocStrat == MEM_UNDEF)
	{
        MMS_IL_PRINT("%d: No such allocation method!\n", marshalCtx->nAllocMethod);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

        MMS_IL_PRINT("0\n");
        return TIMM_OSAL_ERR_NONE;
}