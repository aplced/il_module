#ifndef __CTransferBufferStream2Host__H__
#define __CTransferBufferStream2Host__H__

#include "inc/lib_comm.h"
#include "inc/TransferBuffer/CTransferBuffer.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

class CTransferBufferStream2Host : public CTransferBuffer
{
public:

    TIMM_OSAL_ERRORTYPE Init(COMXComponent* pComp, int associatedPort, int totalDataSize, 
                                                        void* pServiceHeaderStreamBuffer);
    TIMM_OSAL_ERRORTYPE Transfer(void* dataPtr);
    void TransferLine(void* dataPtr, int dataSize, int status);

private:
    dataTransferMessage_t dataOwner;
    dataTransferMessageFull_t msg;
    COMXComponent* ipComp;
};

#endif