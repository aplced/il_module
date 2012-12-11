#ifndef __CComponentDispatcherMarshalMemReadHandle__H__
#define __CComponentDispatcherMarshalMemReadHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalMemReadHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemReadHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif