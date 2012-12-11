#ifndef __CComponentDispatcherGetParamHandle__H__
#define __CComponentDispatcherGetParamHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"


class CComponentDispatcherGetParamHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetParamHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif