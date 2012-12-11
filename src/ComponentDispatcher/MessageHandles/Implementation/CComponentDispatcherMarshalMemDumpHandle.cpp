#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemDumpHandle.h"
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalMemDumpHandle::Process(void* pMessage)
{
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;
    COMXComponent *pComp;
    marshalMemAllocMessage_t* marshalCtx = ((marshalMemAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload);
    void* dataBuffPtr = NULL;
    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);

    ServiceHeaderSaveBuffer_t ServiceHeaderSaveBuffer;

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (pComp->nMemMarshallingListSz <= 0)
	{
        MMS_IL_PRINT("nMemMarshallingListSz is zero! No memory to dump\n");
        return TIMM_OSAL_ERR_NONE;
    }//if (pComp->nMemMarshallingListSz > 0)

    for (unsigned int i = 0; i < pComp->nMemMarshallingListSz; i++)
	{
        if (pComp->pMemMarshalling[i].pA9 == marshalCtx->nPtr)
		{
            CTransferBufferSave2File SaveBuffer;
            if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)
			{
				CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                // dataBuffPtr = marshalCtx->nPtr;
                dataBuffPtr = (TIMM_OSAL_U8*)ionAlloc->getMappedAddr(marshalCtx->nPtr);
                if (dataBuffPtr == NULL)
				{
                    return TIMM_OSAL_ERR_UNKNOWN;
                }
            }
			else
			{
                dataBuffPtr = (TIMM_OSAL_U8*)marshalCtx->nPtr;
            }// if (pComp->tMemMarshallingAllocStrat == MEM_ION_1D)

            strcpy(ServiceHeaderSaveBuffer.namePrfx,"\0");
            strcpy(ServiceHeaderSaveBuffer.fileExt,"\0");

            error += SaveBuffer.Init(pComp, (unsigned int) marshalCtx->nPtr, marshalCtx->nSize, &ServiceHeaderSaveBuffer);
            error += SaveBuffer.Transfer(dataBuffPtr);

            if (TIMM_OSAL_ERR_NONE == error)
			{
                MMS_IL_PRINT("0\n");

                return TIMM_OSAL_ERR_NONE;
            }
			else
			{
                MMS_IL_PRINT("Memory Dump failed!\n");

                return error;
            }//if (TIMM_OSAL_ERR_NONE == error)

        }//if (pComp->pMemMarshalling[i].pA9 == marshalCtx->nPtr)
    }//for (int i = 0; i < pComp->nMemMarshallingListSz; i++)

    MMS_IL_PRINT("Component has no associated allocated memory with address %p!\n", marshalCtx->nPtr);
    return TIMM_OSAL_ERR_UNKNOWN;
}