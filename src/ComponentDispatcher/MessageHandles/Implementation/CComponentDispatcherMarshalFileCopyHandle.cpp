#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalFileCopyHandle.h"
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherMarshalFileCopyHandle::Process(void* pMessage)
{
    COMXComponent *pComp;
    marshalFileCopyMessage_t* marshalCtx = ((marshalFileCopyMessage_t *)((systemMessage_t *)pMessage)->pPayload);

    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(marshalCtx->nComponentId);
    unsigned int nPortIdx = marshalCtx->nPortNum;
    unsigned int nBuffIdx = marshalCtx->nBuffNum;
    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    void* pBuf;
    char file_name[255];
    memset(file_name, 0, 255);
    sprintf(file_name, "%s", marshalCtx->cFileName);
    MMS_IL_PRINT("Calling copyFile2Buffer for file %s\n", file_name);

    COMXPortHandle* portData = pComp->GetPortData(nPortIdx);
    //pBuf = portData->pComponentBuffers[nBuffIdx].pBuffer->pBuffer;
    CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
    pBuf = ionAlloc->getMappedAddr(portData->pComponentBuffers[nBuffIdx].pBuffer->pBuffer);
    if (pBuf == NULL)
    {
        MMS_IL_PRINT("Buffer %d  on port %d is NULL\n", nBuffIdx, nPortIdx);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    //Copy a file to a buffer
    marshalCtx->nSize = copyFile2Buffer(file_name, pBuf, marshalCtx->nSize);
    if (0 < marshalCtx->nSize)
	{
        portData->pComponentBuffers[nBuffIdx].bFree = TIMM_OSAL_TRUE;
       // portData->pComponentBuffers[nBuffIdx].pBuffer->nFlags = OMX_BUFFERFLAG_EOS;
        portData->pComponentBuffers[nBuffIdx].pBuffer->nFilledLen = marshalCtx->nSize;

        MMS_IL_PRINT("Successfully copied %d bytes\n", marshalCtx->nSize);
        return TIMM_OSAL_ERR_NONE;
    } else
    {
        MMS_IL_PRINT("Error when copying\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	return TIMM_OSAL_ERR_NONE;
}