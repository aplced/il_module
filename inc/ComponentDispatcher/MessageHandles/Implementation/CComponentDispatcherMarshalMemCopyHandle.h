#ifndef __CComponentDispatcherMarshalMemCopyHandle__H__
#define __CComponentDispatcherMarshalMemCopyHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalMemCopyHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemCopyHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif