#ifndef __CComponentDispatcherReleaseHandleHandle__H__
#define __CComponentDispatcherReleaseHandleHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherReleaseHandleHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherReleaseHandleHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif