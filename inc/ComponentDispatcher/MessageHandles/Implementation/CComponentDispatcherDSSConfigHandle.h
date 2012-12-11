#ifndef __CComponentDispatcherDSSConfigHandle__H__
#define __CComponentDispatcherDSSConfigHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherDSSConfigHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherDSSConfigHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif