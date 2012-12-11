#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemCopyHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemCopyHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    marshalMemAllocMessage_t* marshalCtx = ((marshalMemAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload);
     void * ptr = NULL;
    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
   // MMS_IL_PRINT("marshalCtx->nPtr 0x%X\n",marshalCtx->nPtr);
    if (pComp->nMemMarshallingListSz > 0)
	{
        unsigned int i;

        for (i = 0; i < pComp->nMemMarshallingListSz; i++)
		{
           // MMS_IL_PRINT("pComp->pMemMarshalling[i].pA9 0x%X\n",pComp->pMemMarshalling[i].pA9);
            if (pComp->pMemMarshalling[i].pA9 == marshalCtx->nPtr)
			{
                if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
                {
                    CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                    MMS_IL_PRINT("At address %p with offset %d and size %d\n", marshalCtx->nPtr, marshalCtx->nOffset, marshalCtx->nSize);
                    ptr = ionAlloc->getMappedAddr(marshalCtx->nPtr);
                    //ptr = marshalCtx->nPtr;
                } else if (pComp->tMemMarshallingAllocStrat == MEM_OSAL)
                {
                    ptr = pComp->pMemMarshalling[i].pA9;
                }
                memcpy((void*) ((unsigned int) ptr + marshalCtx->nOffset), (void*) (marshalCtx + sizeof(marshalMemAllocMessage_t)), marshalCtx->nSize);

                MMS_IL_PRINT("0\n");
                return TIMM_OSAL_ERR_NONE;
            }
        }

        MMS_IL_PRINT("Component has no associated allocated memory with address %p!\n", marshalCtx->nPtr);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    MMS_IL_PRINT("Component has no associated allocated memory at all!\n");
    return TIMM_OSAL_ERR_UNKNOWN;
}