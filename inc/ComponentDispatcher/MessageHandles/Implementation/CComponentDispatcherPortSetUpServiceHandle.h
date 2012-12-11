#ifndef __CComponentDispatcherPortSetUpServiceHandle__H__
#define __CComponentDispatcherPortSetUpServiceHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherPortSetUpServiceHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortSetUpServiceHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif