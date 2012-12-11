#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocHandle.h"
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemAllocHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    unsigned int i;
    marshalMemAllocMessage_t* marshalCtx = ((marshalMemAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload);

    marshalCtx->nPtr = NULL;

    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (pComp->nMemMarshallingListSz == 0)
	{
        pComp->pMemMarshalling = (marshalMem_t*)malloc(STRUCT_ACQ_ARRAY_SZ * sizeof(marshalMem_t));
        if (pComp->pMemMarshalling == NULL)
		{
            MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
            return TIMM_OSAL_ERR_ALLOC;
        }
        pComp->nMemMarshallingListSz += STRUCT_ACQ_ARRAY_SZ;

        for (i = 0; i < pComp->nMemMarshallingListSz; i++)
		{
            pComp->pMemMarshalling[i].pA9 = NULL;
        }
    }

    for (i = 0; i < pComp->nMemMarshallingListSz; i++)
	{
        if (pComp->pMemMarshalling[i].pA9 == NULL)
        {
            if (pComp->tMemMarshallingAllocStrat == MEM_OSAL)
			{
                pComp->pMemMarshalling[i].pA9 = malloc(marshalCtx->nSize);

            }else if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
            {
               // MMS_IL_PRINT("MEM_ION_1D\n");
				CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                TIMM_OSAL_U32 nSize = ALIGN(marshalCtx->nSize, 4096);
                 //void* prt = ionAlloc->alloc(nSize, 4096);
                 //pComp->pMemMarshalling[i].pA9 = ionAlloc->getMappedAddr(prt);
                 pComp->pMemMarshalling[i].pA9 = ionAlloc->alloc(nSize, 4096, 0);
               //  MMS_IL_PRINT("marshalCtx->nSize %d\n", nSize);
            }

            if (pComp->pMemMarshalling[i].pA9 == NULL)
			{
                MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
                return TIMM_OSAL_ERR_ALLOC;
            }
            break;
        }
    }

    //if MemMarshallingList is full
    if (i == pComp->nMemMarshallingListSz)
	{
        void* tmpPtr = malloc((pComp->nMemMarshallingListSz + STRUCT_ACQ_ARRAY_SZ)*sizeof(marshalMem_t));
        if (tmpPtr == NULL)
		{
            MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
            return TIMM_OSAL_ERR_ALLOC;
        }
        memcpy(tmpPtr, pComp->pMemMarshalling, pComp->nMemMarshallingListSz*sizeof(marshalMem_t));
        free(pComp->pMemMarshalling);
        pComp->pMemMarshalling = (marshalMem_t*)tmpPtr;
        pComp->nMemMarshallingListSz += STRUCT_ACQ_ARRAY_SZ;

        if (pComp->tMemMarshallingAllocStrat == MEM_OSAL)
		{
            pComp->pMemMarshalling[i].pA9 = malloc(marshalCtx->nSize);
        }else if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
		{
			CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
            pComp->pMemMarshalling[i].pA9 = ionAlloc->alloc(marshalCtx->nSize, 4096, 0);
        }

        if (pComp->pMemMarshalling[i].pA9 == NULL)
		{
            MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
            return TIMM_OSAL_ERR_ALLOC;
        }
    }

    marshalCtx->nPtr = pComp->pMemMarshalling[i].pA9;

    MMS_IL_PRINT("0\n");
    return TIMM_OSAL_ERR_NONE;
}