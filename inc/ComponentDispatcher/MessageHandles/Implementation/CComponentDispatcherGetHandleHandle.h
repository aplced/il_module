#ifndef __CComponentDispatcherGetHandleHandle__H__
#define __CComponentDispatcherGetHandleHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherGetHandleHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetHandleHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif