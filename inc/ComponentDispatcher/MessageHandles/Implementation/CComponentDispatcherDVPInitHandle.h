#ifndef __CComponentDispatcherDVPInitHandle__H__
#define __CComponentDispatcherDVPInitHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPInitHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPInitHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif