#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDisconnectTCPHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDisconnectTCPHandle::Process(void* pMessage)
{
    pMessage = pMessage; // warning: unused parameter

    //Stop Communication Channel
    printf("Disconnect Message is received from AL. Stopping IL processes...\n");
    dispatcher->CloseChannel();
    return TIMM_OSAL_ERR_NONE;
}

