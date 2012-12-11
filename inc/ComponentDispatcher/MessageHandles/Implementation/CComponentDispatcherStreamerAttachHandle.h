#ifndef __CComponentDispatcherStreamerAttachHandle__H__
#define __CComponentDispatcherStreamerAttachHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherStreamerAttachHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherStreamerAttachHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif