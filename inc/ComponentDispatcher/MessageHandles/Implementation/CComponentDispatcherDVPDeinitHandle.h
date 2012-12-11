#ifndef __CComponentDispatcherDVPDeinitHandle__H__
#define __CComponentDispatcherDVPDeinitHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPDeinitHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPDeinitHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif