#ifndef __CComponentDispatcherMarshalMemAllocHandle__H__
#define __CComponentDispatcherMarshalMemAllocHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalMemAllocHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemAllocHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif