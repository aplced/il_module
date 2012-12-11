#include "inc/PortService/Implementation/COMXPortServiceStreamBuffer.h"


TIMM_OSAL_ERRORTYPE COMXPortServiceStreamBuffer::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
    TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
    if(err != TIMM_OSAL_ERR_NONE)
    {
        MMS_IL_PRINT("Failed to init StreamBuffer service\n");
        return err;
    }
    pServiceHeaderStreamBuffer = (ServiceHeaderStreamBuffer_t*)pServiceHeader;

   // OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

    FrmData = (FrmData_t*)malloc(sizeof(FrmData_t));
    if (FrmData == NULL)
    {
        return TIMM_OSAL_ERR_ALLOC;
    }

    portData = pComp->GetPortData(associatedPort);
    if(portData == NULL)
    {
        MMS_IL_PRINT("Failed to get port %d data\n", associatedPort);
        return TIMM_OSAL_ERR_OMX;
    }
    switch (portData->tPortDef.eDomain)
	{
        case OMX_PortDomainAudio:
        case OMX_PortDomainOther:
            FrmData->nStride = 0;
            szPtrBuf1 =portData->tPortDef.nBufferSize;
            break;
        case OMX_PortDomainVideo:
            FrmData->nWidth = portData->tPortDef.format.video.nFrameWidth;
            FrmData->nHeight = portData->tPortDef.format.video.nFrameHeight;
            FrmData->nStride = portData->tPortDef.format.video.nStride;
            switch(portData->tPortDef.format.video.eColorFormat)
            {
                case OMX_COLOR_FormatYUV420SemiPlanar:
                case OMX_COLOR_FormatYUV420PackedSemiPlanar:
                    FrmData->nBpp = 1;
                    szPtrBuf1 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight;
                    szPtrBuf2 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight/2;
                    break;
                case OMX_COLOR_FormatCbYCrY:
                    FrmData->nBpp = 2;
                    szPtrBuf1 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight;
                    szPtrBuf2 = 0;
                    break;
                default:
                    FrmData->nStride = 0;
                    break;
            }
            break;
        case OMX_PortDomainImage:
            FrmData->nWidth = portData->tPortDef.format.image.nFrameWidth;
            FrmData->nHeight = portData->tPortDef.format.image.nFrameHeight;
            FrmData->nStride = portData->tPortDef.format.image.nStride;
            switch (portData->tPortDef.format.image.eColorFormat)
            {
                case OMX_COLOR_FormatYUV420SemiPlanar:
                case OMX_COLOR_FormatYUV420PackedSemiPlanar:
                    FrmData->nBpp = 1;
                    szPtrBuf1 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight;
                    // TODO: temporary fix, need proper way to discern between 1D and 2D buffers here!
                    szPtrBuf1 += szPtrBuf1/2;
                    szPtrBuf2 = 0;
                    break;
                case OMX_COLOR_FormatCbYCrY:
                    FrmData->nBpp = 2;
                    szPtrBuf1 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight;
                    szPtrBuf2 = 0;
                    break;
                case OMX_COLOR_Format32bitARGB8888:
                    FrmData->nBpp = 4;
                    szPtrBuf1 = FrmData->nWidth*FrmData->nBpp*FrmData->nHeight;
                    szPtrBuf2 = 0;
                    break;
                default:
                    FrmData->nStride = 0;
                    break;
            }
            break;
        default:
            MMS_IL_PRINT("OMX port domain unknown\n");
            return OMX_ErrorUndefined;
    }

    sem_init(&frameSendMutex, 0, 1);

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceStreamBuffer::Deinit()
{
    sem_wait(&frameSendMutex);

    if(FrmData == NULL)
    {
        printf("Preview frame capture: service not initialized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    free(FrmData);
    sem_destroy(&frameSendMutex);
    FrmData = NULL;

    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceStreamBuffer::ConfigPortService()
{
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceStreamBuffer::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
    if(!StartExecute())
    {
        printf("StreamPrvFrame: service not STARTED!\n");
        return TIMM_OSAL_ERR_NONE;
    }
   // dataTransferMessage_t dataOwner;
    void* pBuf1;
    int dBuf1Stride;
    void* pBuf2;
    int dBuf2Stride;
    CTransferBufferStream2Host Streamer;// = new CTransferBufferStream2Host;

    if(FrmData == NULL)
    {
        printf("Preview frame capture: service not initialized!\n");
        StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    sem_wait(&frameSendMutex);

    GetMemoryPtrs(eAllocType, pBufHdr, &pBuf1, &dBuf1Stride, &pBuf2, &dBuf2Stride);

    if(dBuf1Stride == -1)
    {
        dBuf1Stride = FrmData->nStride;
    }
    if(dBuf2Stride == -1)
    {
        dBuf2Stride = FrmData->nStride;
    }
    if(portData->tPortDef.eDomain == OMX_PortDomainImage)
    {
        szPtrBuf1 = pBufHdr->nFilledLen;
        szPtrBuf2 = 0;
        dBuf1Stride = 0;
        dBuf2Stride = 0;
    }
    MMS_IL_PRINT("Reading from 0x%08X with offset %d and filled len %d\n", (unsigned int)pBuf1,
                                                                                    (int)pBufHdr->nOffset,
                                                                                    (int)pBufHdr->nFilledLen);

    unsigned char* dataPtr = (unsigned char*)(pBuf1) + pBufHdr->nOffset;
    // int imgSize = FrmData->nWidth * FrmData->nHeight * FrmData->nBpp;
    //send data owner
    Streamer.Init(pComp, associatedPort,szPtrBuf1 + szPtrBuf2, pServiceHeaderStreamBuffer);

    MMS_IL_PRINT("szPtrBuf1 - %d, szPtrBuf2 - %d\n", szPtrBuf1, szPtrBuf2);

    void* tmpBuff = malloc(szPtrBuf1 + szPtrBuf2);
    char* tmpBuffWalker = (char*)tmpBuff;

    int status = MSG_STATUS_DATA_START;
    unsigned int writtenBytes = 0;

    if (dBuf1Stride == 0)
    {//if no border available, applicable for jpeg saving
         memcpy(tmpBuffWalker, dataPtr, szPtrBuf1);
    } else
    {
        while (writtenBytes < szPtrBuf1)
        {
            //TransferLine(dataPtr,  FrmData->nBpp*FrmData->nWidth, status);
            memcpy(tmpBuffWalker, dataPtr, FrmData->nBpp*FrmData->nWidth);
            tmpBuffWalker += FrmData->nBpp*FrmData->nWidth;

            dataPtr += dBuf1Stride;
            writtenBytes += FrmData->nBpp*FrmData->nWidth;

            if(writtenBytes < (szPtrBuf1 - FrmData->nBpp*FrmData->nWidth))
            {
                status = MSG_STATUS_DATA_MORE;
            }
            else if(szPtrBuf2 == 0)
            {
                MMS_IL_PRINT("End of message (one buffer)\n");
                status = MSG_STATUS_DATA_END;
            }
        }
    }

    dataPtr = (unsigned char*)(pBuf2) + pBufHdr->nOffset;
    writtenBytes = 0;
    if (dBuf2Stride == 0)
    { //if no border available, applicable for jpeg saving
        memcpy(tmpBuffWalker, dataPtr, szPtrBuf2);
    } else
    {
        while (writtenBytes < szPtrBuf2)
        {
            //TransferLine(dataPtr,  FrmData->nBpp*FrmData->nWidth, status);
            memcpy(tmpBuffWalker, dataPtr, FrmData->nBpp*FrmData->nWidth);
            tmpBuffWalker += FrmData->nBpp*FrmData->nWidth;

            dataPtr += dBuf2Stride;
            writtenBytes += FrmData->nBpp*FrmData->nWidth;

            if(writtenBytes < (szPtrBuf2 - FrmData->nBpp*FrmData->nWidth))
            {
                status = MSG_STATUS_DATA_MORE;
            }
            else
            {
                MMS_IL_PRINT("End of message (two buffers)\n");
                status = MSG_STATUS_DATA_END;
            }
        }
    }

    Streamer.Transfer(tmpBuff);
    free(tmpBuff);

    FrmData->nFrmCount++;

    sem_post(&frameSendMutex);

    StopExecute();
   // delete(Streamer);
    return TIMM_OSAL_ERR_NONE;
}