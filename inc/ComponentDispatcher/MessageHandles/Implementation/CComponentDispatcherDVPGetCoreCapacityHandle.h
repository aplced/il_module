#ifndef __CComponentDispatcherDVPGetCoreCapacityHandle__H__
#define __CComponentDispatcherDVPGetCoreCapacityHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPGetCoreCapacityHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPGetCoreCapacityHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif