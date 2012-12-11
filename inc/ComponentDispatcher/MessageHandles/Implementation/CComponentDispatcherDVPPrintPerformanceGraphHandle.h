#ifndef __CComponentDispatcherDVPPrintPerformanceGraphHandle__H__
#define __CComponentDispatcherDVPPrintPerformanceGraphHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPPrintPerformanceGraphHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPPrintPerformanceGraphHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif