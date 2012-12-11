#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemFreeHandle.h"
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemFreeHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    marshalMemAllocMessage_t* marshalCtx = ((marshalMemAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload);

    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (pComp->nMemMarshallingListSz > 0)
	{
        unsigned int i;
        unsigned int nFreeCells = 0;
        unsigned int bMatch = 0;

        for (i = 0; i < pComp->nMemMarshallingListSz; i++)
		{
            if (pComp->pMemMarshalling[i].pA9 == marshalCtx->nPtr)
			{

                if (pComp->tMemMarshallingAllocStrat == MEM_OSAL)
				{
                    free(pComp->pMemMarshalling[i].pA9);
                }
				else if(pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
				{
					CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                    ionAlloc->free(pComp->pMemMarshalling[i].pA9);
                }
                pComp->pMemMarshalling[i].pA9 = NULL;
                nFreeCells++;
                bMatch = 1;
            }
            else if(pComp->pMemMarshalling[i].pA9 == NULL)
			{
                nFreeCells++;
            }
        }

        if (nFreeCells == pComp->nMemMarshallingListSz)
		{
            pComp->nMemMarshallingListSz = 0;
            free(pComp->pMemMarshalling);
            pComp->pMemMarshalling = NULL;
        }

        if (bMatch)
		{
            MMS_IL_PRINT("0\n");
            return TIMM_OSAL_ERR_NONE;
        } 
		else 
		{
            MMS_IL_PRINT("Component has no associated allocated memory with address %p!\n", marshalCtx->nPtr);
            return TIMM_OSAL_ERR_UNKNOWN;
        }//if (bMatch)
    }//if (pComp->nMemMarshallingListSz > 0)

    MMS_IL_PRINT("Component has no associated allocated memory at all!\n");
    return TIMM_OSAL_ERR_UNKNOWN; 
}