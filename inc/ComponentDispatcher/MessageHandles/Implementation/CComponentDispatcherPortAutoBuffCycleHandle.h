#ifndef __CComponentDispatcherPortAutoBuffCycleHandle__H__
#define __CComponentDispatcherPortAutoBuffCycleHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherPortAutoBuffCycleHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherPortAutoBuffCycleHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif