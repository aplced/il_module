#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CReadHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherI2CReadHandle::Process(void* pMessage)
{
    //unsigned int nPayloadSize = ((systemMessage_t *)pMessage)->nPayloadSize;
    struct i2c_msg msg;
    TIMM_OSAL_U8* data = NULL;

    I2CMessage_t* pI2CMessage = (I2CMessage_t*)((systemMessage_t *)pMessage)->pPayload;

    if (dispatcher->I2C_device <= 0 )
    {
        MMS_IL_PRINT("Device is closed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    I2CData_t* pI2CData = (I2CData_t*)(pI2CMessage->pData);
    if (pI2CData == NULL)
    {
        MMS_IL_PRINT("pI2CMessage is NULL\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

   int SubAddr = pI2CData->nSubAddr;
   if (TIMM_OSAL_ERR_NONE != set_slave_addr(dispatcher->I2C_device, pI2CMessage->nSlaveAddr,SubAddr ))
    {
        MMS_IL_PRINT("Slave address not set!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    MMS_IL_PRINT("Reading from slave addres 0x%04X\n", pI2CMessage->nSlaveAddr);

   int pldSz = pI2CData->nPayloadSize;
   data = (TIMM_OSAL_U8*)malloc(pldSz * sizeof(TIMM_OSAL_U8));
   if (data == NULL)
   {
        MMS_IL_PRINT("Error allocating memory!\n");
        return TIMM_OSAL_ERR_ALLOC;
   }
   memset(data, 0, pldSz);

   MMS_IL_PRINT("Reading from Subaddres 0x%04X, %d bytes\n", SubAddr, pldSz);
   msg.addr = pI2CMessage->nSlaveAddr;
   msg.flags = I2C_M_RD;
   msg.len = pldSz;
   msg.buf = data;

   if (TIMM_OSAL_ERR_NONE != i2c_transfer(dispatcher->I2C_device, &msg))
   {
      MMS_IL_PRINT("i2c_smbus_read Failed\n");
      return TIMM_OSAL_ERR_UNKNOWN;
   }

   for(int i = pldSz - 1, j = 0; i >= 0; i--, j++)
   {
       pI2CData->pPayloadData[i] = (TIMM_OSAL_U8)msg.buf[j];
       //MMS_IL_PRINT("msg.buf 0x%08X\n", msg.buf[j]);
       //MMS_IL_PRINT("pI2CData->pPayloadData 0x%08X\n", pI2CData->pPayloadData[i]);
   }

   free(data);
   MMS_IL_PRINT("0\n")
   return TIMM_OSAL_ERR_NONE;
}