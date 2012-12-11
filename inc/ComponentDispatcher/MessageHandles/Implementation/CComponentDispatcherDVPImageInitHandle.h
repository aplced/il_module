#ifndef __CComponentDispatcherDVPImageInitHandle__H__
#define __CComponentDispatcherDVPImageInitHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPImageInitHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPImageInitHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif