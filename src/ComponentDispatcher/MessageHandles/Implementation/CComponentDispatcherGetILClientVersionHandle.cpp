#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetILClientVersionHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetILClientVersionHandle::Process(void* pMessage)
{
    ((ilClientVersionMessage_t*)((systemMessage_t*)pMessage)->pPayload)->nVersion = VER;
    return TIMM_OSAL_ERR_NONE;
}