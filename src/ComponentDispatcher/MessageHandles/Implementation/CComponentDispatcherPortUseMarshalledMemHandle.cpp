#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortUseMarshalledMemHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortUseMarshalledMemHandle::Process(void* pMessage)
{
    marshalMemUseOnPortMessage_t* memUseCtx = ((marshalMemUseOnPortMessage_t *)((systemMessage_t *)pMessage)->pPayload);
    COMXComponent* pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(memUseCtx->nComponentId);

    if (NULL == pComp)
    {
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    return UseMarshalledMemOnComponentPortBuffers(pComp, memUseCtx->nPortNumber, memUseCtx->nBufNumber, &(memUseCtx->nBufPtr));
}