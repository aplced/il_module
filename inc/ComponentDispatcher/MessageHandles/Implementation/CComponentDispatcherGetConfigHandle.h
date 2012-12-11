#ifndef __CComponentDispatcherGetConfigHandle__H__
#define __CComponentDispatcherGetConfigHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherGetConfigHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetConfigHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif