#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPSetCoreCapacityHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPSetCoreCapacityHandle::Process(void* pMessage)
{
   DVP_CoreCapacityMessage_t* pCoreCapacity = (DVP_CoreCapacityMessage_t *)((systemMessage_t *)pMessage)->pPayload;
   CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pCoreCapacity->nComponentID);

   DVPComponentHandle->Set_Core_Capacity(pCoreCapacity->nCore, pCoreCapacity->capacity);
   MMS_IL_PRINT("Capacity of core[%d] is set to %u!\n",pCoreCapacity->nCore, pCoreCapacity->capacity);
   
   return TIMM_OSAL_ERR_NONE;
}