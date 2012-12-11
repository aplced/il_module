#ifndef __CComponentDispatcherSetParamHandle__H__
#define __CComponentDispatcherSetParamHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherSetParamHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherSetParamHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif