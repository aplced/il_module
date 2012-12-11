#ifndef __CComponentDispatcherI2CWriteHandle__H__
#define __CComponentDispatcherI2CWriteHandle__H__
#include "inc/Communication/I2C/CCommunicationI2C.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"

class CComponentDispatcherI2CWriteHandle : public CComponentDispatcherMessageHandle
{
public:
    CComponentDispatcherI2CWriteHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId) : CComponentDispatcherMessageHandle(hDispatcher, hId){};
    TIMM_OSAL_ERRORTYPE Process(void* pMessage);
private:

};

#endif