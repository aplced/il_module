#ifndef __CComponentDispatcherDVPInitParamsHandle__H__
#define __CComponentDispatcherDVPInitParamsHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPInitParamsHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPInitParamsHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif