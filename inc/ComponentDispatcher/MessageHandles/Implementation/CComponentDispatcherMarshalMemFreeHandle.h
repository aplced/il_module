#ifndef __CComponentDispatcherMarshalMemFreeHandle__H__
#define __CComponentDispatcherMarshalMemFreeHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalMemFreeHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalMemFreeHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif