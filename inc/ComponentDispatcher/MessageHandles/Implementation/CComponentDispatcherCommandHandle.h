#ifndef __CComponentDispatcherCommandHandle__H__
#define __CComponentDispatcherCommandHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherCommandHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherCommandHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif