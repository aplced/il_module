#ifndef __CComponentDispatcherReleaseI2CHandleHandle__H__
#define __CComponentDispatcherReleaseI2CHandleHandle__H__
#include "inc/Communication/I2C/CCommunicationI2C.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherReleaseI2CHandleHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherReleaseI2CHandleHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
};

#endif