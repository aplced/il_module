#ifndef __CComponentDispatcherPortUseMarshalledMemHandle__H__
#define __CComponentDispatcherPortUseMarshalledMemHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"
#include "inc/TransferBuffer/Save2File/CTransferBufferSave2File.h"

class CComponentDispatcherPortUseMarshalledMemHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortUseMarshalledMemHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif