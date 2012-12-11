#ifndef __CComponentDispatcherDVPKernelGraphDeinitHandle__H__
#define __CComponentDispatcherDVPKernelGraphDeinitHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPKernelGraphDeinitHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPKernelGraphDeinitHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif