#ifndef __CComponentDispatcherGetI2CHandleHandle__H__
#define __CComponentDispatcherGetI2CHandleHandle__H__

#include "inc/Communication/I2C/CCommunicationI2C.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"


class CComponentDispatcherGetI2CHandleHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherGetI2CHandleHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif