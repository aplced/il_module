#ifndef __CComponentDispatcherSetPortBufferCountHandle__H__
#define __CComponentDispatcherSetPortBufferCountHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherSetPortBufferCountHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherSetPortBufferCountHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif