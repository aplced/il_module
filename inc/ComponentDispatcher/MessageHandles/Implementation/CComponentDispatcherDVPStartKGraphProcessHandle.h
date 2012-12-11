#ifndef __CComponentDispatcherDVPStartKGraphProcessHandle__H__
#define __CComponentDispatcherDVPStartKGraphProcessHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPStartKGraphProcessHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPStartKGraphProcessHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif