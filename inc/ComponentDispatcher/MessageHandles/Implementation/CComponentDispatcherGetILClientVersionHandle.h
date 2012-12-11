#ifndef __CComponentDispatcherGetILClientVersionHandle__H__
#define __CComponentDispatcherGetILClientVersionHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherGetILClientVersionHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetILClientVersionHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif