#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetI2CHandleHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetI2CHandleHandle::Process(void* pMessage)
{
    //int I2C_device = dispatcher->I2C_device;
    unsigned int nPayloadSize = ((systemMessage_t *)pMessage)->nPayloadSize;
    char pI2C_filename[20];

    if (nPayloadSize>0)
    {
        char* filename = (char*)((systemMessage_t *)pMessage)->pPayload;
        strcpy(pI2C_filename, filename);
    } else
    {
        MMS_IL_PRINT("Using edfault device %s!!!\n", I2C_2_DEVICE);
        strcpy(pI2C_filename, I2C_2_DEVICE);
    }

    if (dispatcher->I2C_device <= 0)
    {
        dispatcher->I2C_device = open(pI2C_filename, O_RDWR);
    } else
    {
        MMS_IL_PRINT("File %d is already opened. Please close all devices first!\n", dispatcher->I2C_device);
        return TIMM_OSAL_ERR_NONE;
    }

    if (dispatcher->I2C_device <= 0)
    {
        MMS_IL_PRINT("Couldn't open %s device\n", pI2C_filename);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    MMS_IL_PRINT("Open '%s' device on file %d\n", pI2C_filename, dispatcher->I2C_device);

    return TIMM_OSAL_ERR_NONE;
}