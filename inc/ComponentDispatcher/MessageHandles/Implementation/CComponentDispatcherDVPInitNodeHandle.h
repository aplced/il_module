#ifndef __CComponentDispatcherDVPInitNodeToHandle__H__
#define __CComponentDispatcherDVPInitNodeHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPInitNodeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPInitNodeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif
