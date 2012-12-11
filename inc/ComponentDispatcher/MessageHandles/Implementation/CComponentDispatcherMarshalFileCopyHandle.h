#ifndef __CComponentDispatcherMarshalFileCopyHandle__H__
#define __CComponentDispatcherMarshalFileCopyHandle__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherMarshalFileCopyHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherMarshalFileCopyHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif