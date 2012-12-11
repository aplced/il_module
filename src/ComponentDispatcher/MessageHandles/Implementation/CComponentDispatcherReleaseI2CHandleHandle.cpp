#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseI2CHandleHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherReleaseI2CHandleHandle::Process(void* pMessage)
{
    pMessage = pMessage; // warning: unused parameter
    if (dispatcher->I2C_device <= 0)
    {
        MMS_IL_PRINT("Device %d is alreadyclosed\n", dispatcher->I2C_device);
        return TIMM_OSAL_ERR_NONE;
    }

    if (close(dispatcher->I2C_device) != 0)
    {
        MMS_IL_PRINT("Device %d is not closed\n", dispatcher->I2C_device);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    dispatcher->I2C_device = 0;

    return TIMM_OSAL_ERR_NONE;
}