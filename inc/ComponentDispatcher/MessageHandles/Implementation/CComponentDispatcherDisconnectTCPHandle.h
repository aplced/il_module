#ifndef __CComponentDispatcherDisconnectTCPHandle__H__
#define __CComponentDispatcherDisconnectTCPHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDisconnectTCPHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDisconnectTCPHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif