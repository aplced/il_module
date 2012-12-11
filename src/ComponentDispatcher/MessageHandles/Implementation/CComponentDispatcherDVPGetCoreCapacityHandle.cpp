#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPGetCoreCapacityHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPGetCoreCapacityHandle::Process(void* pMessage)
{
   DVP_CoreCapacityMessage_t* pCoreCapacity = (DVP_CoreCapacityMessage_t *)((systemMessage_t *)pMessage)->pPayload;
   CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pCoreCapacity->nComponentID);

   pCoreCapacity->capacity = DVPComponentHandle->Get_Core_Capacity(pCoreCapacity->nCore);
   MMS_IL_PRINT("Capacity of core[%d] is %u!\n",pCoreCapacity->nCore, pCoreCapacity->capacity);
   
   return TIMM_OSAL_ERR_NONE;
}