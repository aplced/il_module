#ifndef __CComponentDispatcherMarshalMemAllocTypeHandle__H__
#define __CComponentDispatcherMarshalMemAllocTypeHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalMemAllocTypeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemAllocTypeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif