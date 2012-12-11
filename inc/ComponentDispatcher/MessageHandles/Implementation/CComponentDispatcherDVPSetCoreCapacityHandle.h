#ifndef __CComponentDispatcherDVPSetCoreCapacityHandle__H__
#define __CComponentDispatcherDVPSetCoreCapacityHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPSetCoreCapacityHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPSetCoreCapacityHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif