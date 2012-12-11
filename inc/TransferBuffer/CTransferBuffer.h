//#pragma once
#ifndef __CTransferBuffer__H__
#define __CTransferBuffer__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

class CTransferBuffer
{
public:
   virtual TIMM_OSAL_ERRORTYPE Init(COMXComponent* pComp, int associatedPort, int totalDataSize, 
                                                        void* pServiceHeaderStreamBuffer) =0;
   virtual TIMM_OSAL_ERRORTYPE Transfer(void* dataPtr) =0;
   virtual ~CTransferBuffer(){};
};

#endif