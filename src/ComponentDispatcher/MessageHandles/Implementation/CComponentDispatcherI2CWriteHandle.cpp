#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CWriteHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherI2CWriteHandle::Process(void* pMessage)
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

    MMS_IL_PRINT("Writing to slave addres 0x%04X\n", pI2CMessage->nSlaveAddr);

    I2CData_t* pI2CData = (I2CData_t*)(pI2CMessage->pData);
    if (pI2CData == NULL)
    {
        MMS_IL_PRINT("pI2CMessage is NULL\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

   int pldSz = pI2CData->nPayloadSize;
   data = (TIMM_OSAL_U8*)malloc((pldSz+2)*sizeof(TIMM_OSAL_U8));
   if (data == NULL)
   {
        MMS_IL_PRINT("Error allocating memory!\n");
        return TIMM_OSAL_ERR_ALLOC;
   }
   memset(data, 0, (pldSz+2));
   int SubAddr = pI2CData->nSubAddr;
   MMS_IL_PRINT("Writing to 0x%04X, %d bytes\n", SubAddr, pldSz);

   msg.addr = pI2CMessage->nSlaveAddr;
   msg.flags = 0x00;
   msg.len = pldSz + 2;
   msg.buf = data;
   data[0] = (SubAddr >> 8) & 0xFF;
   data[1] = SubAddr & 0xFF;
   int start_byte = 2;
    if (data[0] == 0)
    {
         data[0] = data[1];
         msg.len = 1;
         start_byte = 1;
         msg.len--;
    }
   for(int i = (pldSz - 1), j = start_byte; i >= 0; i--, j++)
   {
       data[j] = pI2CData->pPayloadData[i];
      // MMS_IL_PRINT("msg.buf 0x%X\n", msg.buf[j]);
   }
   if (TIMM_OSAL_ERR_NONE != i2c_transfer(dispatcher->I2C_device, &msg))
   {
      MMS_IL_PRINT("I2c_write Failed\n");
      return TIMM_OSAL_ERR_UNKNOWN;
   }

   free(data);

   MMS_IL_PRINT("0\n")
   return TIMM_OSAL_ERR_NONE;
}