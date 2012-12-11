#ifndef __CComponentDispatcherMarshalMemDumpHandle__H__
#define __CComponentDispatcherMarshalMemDumpHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"
#include "inc/TransferBuffer/Save2File/CTransferBufferSave2File.h"
class CComponentDispatcherMarshalMemDumpHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemDumpHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif