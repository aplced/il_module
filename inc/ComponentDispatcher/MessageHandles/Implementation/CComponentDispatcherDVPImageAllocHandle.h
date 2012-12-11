#ifndef __CComponentDispatcherDVPImageAllocHandle__H__
#define __CComponentDispatcherDVPImageAllocHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPImageAllocHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPImageAllocHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif