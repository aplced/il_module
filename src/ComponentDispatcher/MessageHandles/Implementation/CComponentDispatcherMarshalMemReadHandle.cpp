#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemReadHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemReadHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    marshalMemAllocMessage_t* marshalCtx = ((marshalMemAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload);
    void* dataBuffPtr = NULL;
    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (pComp->nMemMarshallingListSz > 0)
	{
        unsigned int i;

        for (i = 0; i < pComp->nMemMarshallingListSz; i++)
		{
            if (pComp->pMemMarshalling[i].pA9 == (TIMM_OSAL_U8*) marshalCtx->nPtr)
			{
                if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
                {
                    CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                    // dataBuffPtr = marshalCtx->nPtr;
                    dataBuffPtr = (TIMM_OSAL_U8*)ionAlloc->getMappedAddr(marshalCtx->nPtr);
                } else  dataBuffPtr = (TIMM_OSAL_U8*)marshalCtx->nPtr;

                MMS_IL_PRINT("At address %p with offset %d and size %d\n", dataBuffPtr, marshalCtx->nOffset, marshalCtx->nSize);
                memcpy((TIMM_OSAL_U8*)marshalCtx + sizeof(marshalMemAllocMessage_t), ((TIMM_OSAL_U8*) dataBuffPtr) + marshalCtx->nOffset, marshalCtx->nSize);

                MMS_IL_PRINT("0\n");
                return TIMM_OSAL_ERR_NONE;
            }
        }

        MMS_IL_PRINT("Component has no associated allocated memory with fd %x!\n", (unsigned int)marshalCtx->nPtr);
        return TIMM_OSAL_ERR_UNKNOWN;
    }//if (pComp->nMemMarshallingListSz > 0)

    MMS_IL_PRINT("Component has no associated allocated memory at all!\n");
    return TIMM_OSAL_ERR_UNKNOWN;
}