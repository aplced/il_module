#ifndef __CComponentDispatcherDSSInitHandle__H__
#define __CComponentDispatcherDSSInitHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDSSInitHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDSSInitHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif