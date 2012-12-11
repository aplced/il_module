#ifndef __CComponentDispatcherGetOMXVersionHandle__H__
#define __CComponentDispatcherGetOMXVersionHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"


class CComponentDispatcherGetOMXVersionHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetOMXVersionHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif