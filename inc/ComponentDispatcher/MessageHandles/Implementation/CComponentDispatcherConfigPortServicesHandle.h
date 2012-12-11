#ifndef __CComponentDispatcherConfigPortServicesHandle__H__
#define __CComponentDispatcherConfigPortServicesHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherConfigPortServicesHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherConfigPortServicesHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif