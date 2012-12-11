#ifndef __CComponentDispatcherPortLinkHandle__H__
#define __CComponentDispatcherPortLinkHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherPortLinkHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortLinkHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif