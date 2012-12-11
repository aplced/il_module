#include "inc/TransferBuffer/Stream2Host/CTransferBufferStream2Host.h"


TIMM_OSAL_ERRORTYPE CTransferBufferStream2Host::Init(COMXComponent* pComp, int associatedPort, int totalDataSize, void* pServiceHeaderStreamBuffer)
{
    //FILE* fdCapImg;
   // dataTransferMessage_t dataOwner;
    systemMessage_t msg2Host;

    //pComp = (COMXXomponent*)GetOMXComponentSlot(nComponentId);
    ipComp = pComp;
    msg2Host.nId = ((ServiceHeaderStreamBuffer_t*)pServiceHeaderStreamBuffer)->nMsgId;
    dataOwner.Initiator.nInitiatorID = pComp->nComponentId;
    dataOwner.Initiator.nInitiatorPort = associatedPort;
    dataOwner.Initiator.nDataType = ((ServiceHeaderStreamBuffer_t*)pServiceHeaderStreamBuffer)->dataTypeID;
    dataOwner.nTotalDataSize = totalDataSize;

    msg2Host.nStatus = MSG_STATUS_OK;
    msg2Host.nPayloadSize = sizeof(dataOwner);
    msg2Host.pPayload = &dataOwner;

    pComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
    printf("Data owner is send!\n");
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CTransferBufferStream2Host::Transfer(void* dataPtr)
{
    systemMessage_t msg2Host;
    int dataSize = dataOwner.nTotalDataSize;
    TIMM_OSAL_U8* pData = NULL;
    pData = (TIMM_OSAL_U8*)dataPtr;

    msg.Initiator.nDataType = dataOwner.Initiator.nDataType;
    msg.Initiator.nInitiatorID = dataOwner.Initiator.nInitiatorID;
    msg.Initiator.nInitiatorPort = dataOwner.Initiator.nInitiatorPort;
    msg.dataBuffPtr = pData;
    int size_struct = sizeof(dataTransferMessageFull_t);
    unsigned int size_payload = dataSize + size_struct;
    unsigned int alwPldSz = 3072;

    if (size_payload <= alwPldSz)
    {
        msg2Host.nStatus = MSG_STATUS_DATA_FULL;
        msg2Host.nPayloadSize = size_payload;
        msg2Host.pPayload = &msg;
        //send buffer at once
        ipComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
    }else
    {//chomp data
        TIMM_OSAL_U32 transferDataBuff = 0;
        //TIMM_OSAL_U32 transferDataFull = 0;
        TIMM_OSAL_U32 alwPldSzBuff = alwPldSz - size_struct;

        msg2Host.nStatus = MSG_STATUS_DATA_START;
        msg2Host.nPayloadSize = alwPldSz;
        msg2Host.pPayload = &msg;

        ipComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
        //transferred data from buffer
        transferDataBuff += alwPldSzBuff;
        //transferred data full payload
        //transferDataFull += alwPldSz;

        while (size_payload - transferDataBuff > alwPldSz)
        //while (dataSize - transferData > alwPldSz - size_struct)
        {
            msg2Host.nStatus = MSG_STATUS_DATA_MORE;
            msg2Host.nPayloadSize = alwPldSz;
            msg.dataBuffPtr = pData + transferDataBuff;

            ipComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
            transferDataBuff += alwPldSzBuff;
           // transferDataFull += alwPldSz;
        }

        msg2Host.nStatus = MSG_STATUS_DATA_END;
        msg2Host.nPayloadSize = dataSize - transferDataBuff;
        msg.dataBuffPtr = pData + transferDataBuff;
        //send last piece;
        ipComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
    }
    printf("Buffer is sent!\n");
    return TIMM_OSAL_ERR_NONE;
}

void CTransferBufferStream2Host::TransferLine(void* dataPtr, int dataSize, int status)
{
    systemMessage_t msg2Host;
    msg2Host.nId = 0x00F00000;
    msg2Host.nStatus = status;
    msg2Host.nPayloadSize = dataSize;
    msg2Host.pPayload = (TIMM_OSAL_U8*)dataPtr;

    ipComp->dispatcher->SendMessage((systemMessage_t*)&msg2Host);
}