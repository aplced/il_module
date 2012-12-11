#ifndef __CComponentDispatcherDVPImageFreeHandle__H__
#define __CComponentDispatcherDVPImageFreeHandle__H__

#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDVPImageFreeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDVPImageFreeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif