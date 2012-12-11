#ifndef __CComponentDispatcherGetPortInfoHandle__H__
#define __CComponentDispatcherGetPortInfoHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherGetPortInfoHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetPortInfoHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif