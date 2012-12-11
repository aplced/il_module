#ifndef __CComponentDispatcherDVPImageConstructHandle__H__
#define __CComponentDispatcherDVPImageConstructHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPImageConstructHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPImageConstructHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif