#include "inc/PortService/COMXPortService.h"

COMXPortService::COMXPortService(int id)
{
    serviceId = id;
    pConfigData = NULL;
}

COMXPortService::~COMXPortService()
{
    if(pConfigData != NULL)
    {
        free(pConfigData);
        pConfigData = NULL;
    }
}

TIMM_OSAL_ERRORTYPE COMXPortService::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
    pServiceHeader   = pServiceHeader;      // stop warning: unused parameter
    nServiceHeadSize = nServiceHeadSize;    // stop warning: unused parameter

    sem_init(&mutex, 0, 1);

    pComp = ipComp;
    associatedPort = iAssociatedPort;

    MMS_IL_PRINT("Init service %d on port %d\n", serviceId, associatedPort);

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortService::Deinit()
{
    MMS_IL_PRINT("Deinit service %d on port %d\n", serviceId, associatedPort);
    //MMS_IL_PRINT("Waiting on service mutex\n");
    sem_wait(&mutex);
    //MMS_IL_PRINT("Destroying service mutex\n");
    sem_destroy(&mutex);

    return TIMM_OSAL_ERR_NONE;
}

int COMXPortService::ServiceId()
{
    return serviceId;
}

unsigned int COMXPortService::Port()
{
    return associatedPort;
}

void COMXPortService::Enable()
{
    MMS_IL_PRINT("Enable service %d on port %d\n", serviceId, associatedPort);
    sem_wait(&mutex);
    //MMS_IL_PRINT("serviceStopping is false\n");
    serviceStopping = false;
    sem_post(&mutex);
}

void COMXPortService::Disable()
{
    MMS_IL_PRINT("Disable service %d on port %d\n", serviceId, associatedPort);
    sem_wait(&mutex);
    //MMS_IL_PRINT("serviceStopping is true\n");
    serviceStopping = true;
    sem_post(&mutex);
}

bool COMXPortService::StartExecute()
{
    MMS_IL_PRINT("StartExecute service %d on port %d\n", serviceId, associatedPort);
    if(serviceStopping)
    {
        MMS_IL_PRINT("StartExecute service is stopping\n");
        return false;
    }
    sem_wait(&mutex);
    //MMS_IL_PRINT("StartExecute service is a go\n");
    return true;
}

void COMXPortService::StopExecute()
{
    MMS_IL_PRINT("StopExecute service %d on port %d\n", serviceId, associatedPort);
    sem_post(&mutex);
}

void COMXPortService::PopulateConfig(void *ipConfig, int size)
{
    sem_wait(&mutex);
    if(pConfigData == NULL && ipConfig != NULL)
    {
        pConfigData = malloc(size);
        memcpy(pConfigData, ipConfig, size);
       // MMS_IL_PRINT("Configuration set - %s, from ipConfig -%s\n", (char*)pConfigData, (char*)ipConfig);
    } else
    {
        free(pConfigData);
        pConfigData = NULL;
    }

    sem_post(&mutex);
}

void COMXPortService::GetMemoryPtrs(memAllocStrat_t eAllocType, OMX_BUFFERHEADERTYPE* pBufHdr, void** bufPtr1, int* buf1Stride, void** bufPtr2, int* buf2Stride)
{
    *bufPtr1 = NULL;
    *bufPtr2 = NULL;
    *buf1Stride = -1;
    *buf2Stride = -1;

    if (eAllocType == MEM_ION_1D)
    {
        CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
        //*bufPtr1 = pBufHdr->pBuffer;
        *bufPtr1 = ionAlloc->getMappedAddr(pBufHdr->pBuffer);
        *buf1Stride = ionAlloc->getBufferStride(pBufHdr->pBuffer);
    }
    else if (eAllocType == MEM_ION_2D)
    {
        CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
        OMX_TI_BUFFERDESCRIPTOR_TYPE* Ion2DBuf = (OMX_TI_BUFFERDESCRIPTOR_TYPE*)(pBufHdr->pBuffer);
       // *bufPtr1 = Ion2DBuf->pBuf[0];

        *bufPtr1 = ionAlloc->getMappedAddr(Ion2DBuf->pBuf[0]);
        *buf1Stride = ionAlloc->getBufferStride(Ion2DBuf->pBuf[0]);
        if (Ion2DBuf->pBuf[1])
        {
           // *bufPtr2 = Ion2DBuf->pBuf[1];
            *bufPtr2 = ionAlloc->getMappedAddr(Ion2DBuf->pBuf[1]);
            *buf2Stride = ionAlloc->getBufferStride(Ion2DBuf->pBuf[1]);
        }
    }

    if(*bufPtr1 == NULL)
    {
        *bufPtr1 = pBufHdr->pBuffer;
    }
}
