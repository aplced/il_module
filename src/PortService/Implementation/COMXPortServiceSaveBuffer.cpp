#include "inc/PortService/Implementation/COMXPortServiceSaveBuffer.h"
#include <string>

TIMM_OSAL_ERRORTYPE COMXPortServiceSaveBuffer::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	 TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	 if(err != TIMM_OSAL_ERR_NONE)
	 {
         MMS_IL_PRINT("Failed to init SaveBuffer service\n");
         return err;
	 }

    if (pServiceHeader != NULL)
    {
         char* ptr = (char*)pServiceHeader;
         strcpy(ServiceHeaderSaveBuffer.namePrfx, ptr);
         int len = strlen(ServiceHeaderSaveBuffer.namePrfx) + 1;
         ptr += len;
         while (strcmp(ptr, "\0") == 0)
         {
             ptr++;
             len++;
         }
         strcpy(ServiceHeaderSaveBuffer.fileExt, ptr);
         len += strlen(ServiceHeaderSaveBuffer.fileExt) + 1;
         if (len < nServiceHeadSize)
         {//more information is available
            ptr = (char*)pServiceHeader + len;
            strcpy(ServiceHeaderSaveBuffer.filePath, ptr);
         } else memset(ServiceHeaderSaveBuffer.filePath, NULL, 128);
    }
 //  MMS_IL_PRINT("namePrfx %s, fileExt %s !\n", ServiceHeaderSaveBuffer.namePrfx, ServiceHeaderSaveBuffer.fileExt);
   FrmData = (FrmData_t*)malloc(sizeof(FrmData_t));
   if (FrmData == NULL)
   {
       MMS_IL_PRINT("SaveBuffer: service initilization failed!\n");
       return TIMM_OSAL_ERR_UNKNOWN;
   }

	FrmData->nFrmCount = 0;
   // MMS_IL_PRINT("Port %d!\n", associatedPort);
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
            MMS_IL_PRINT("Width - %d, Height - %d, stride - %d, eDomain %d Video \n",FrmData->nWidth,  FrmData->nHeight,  FrmData->nStride, portData->tPortDef.eDomain);
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
            MMS_IL_PRINT("Width - %d, Height - %d, stride - %d, eDomain %d Image \n",FrmData->nWidth,  FrmData->nHeight,  FrmData->nStride, portData->tPortDef.eDomain);
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
    SaveBuffer = new CTransferBufferSave2File;
    bIsSnap = false;
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceSaveBuffer::ConfigPortService()
{
    int err = TIMM_OSAL_ERR_NONE;
    if (pConfigData != NULL)
    {
        MMS_IL_PRINT("Reconfiguring port service Save Buffer!\n");
        if (StartExecute())
        {
            delete(SaveBuffer);
            Init(pComp, associatedPort, NULL, 0);
            memset(ServiceHeaderSaveBuffer.namePrfx, '\0', strlen(ServiceHeaderSaveBuffer.namePrfx));
            char* ptr = (char*)pConfigData;
            strcpy(ServiceHeaderSaveBuffer.namePrfx, ptr);
            MMS_IL_PRINT("new namePrfx - %s, new fileExt - %s!\n", ServiceHeaderSaveBuffer.namePrfx, ServiceHeaderSaveBuffer.fileExt);
            bIsSnap = false;
            if (strcmp(ServiceHeaderSaveBuffer.namePrfx, ptr))
            {
                MMS_IL_PRINT("Setting name prefix failed!\n");
                err =  TIMM_OSAL_ERR_UNKNOWN;
            } else MMS_IL_PRINT("Service is reconfigured!\n");
            StopExecute();
        } else  MMS_IL_PRINT("Service is not enabled or config data is NULL!\n");
    }
    return err;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceSaveBuffer::Deinit()
{
    if(FrmData == NULL)
	{
        MMS_IL_PRINT("Preview frame capture: service not initialized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    delete(SaveBuffer);
    free(FrmData);
	FrmData = NULL;
    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceSaveBuffer::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
	if(!StartExecute())
	{
        MMS_IL_PRINT("SavePrvFrame: service not STARTED!\n");
		return TIMM_OSAL_ERR_NONE;
    }

	void* pBuf1;
	int dBuf1Stride;
	void* pBuf2;
	int dBuf2Stride;
    unsigned char* dataPtr;
    static char oldname[128];

    if(FrmData == NULL)
	{
        MMS_IL_PRINT("Preview frame capture: service not initialized!\n");
		StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	GetMemoryPtrs(eAllocType, pBufHdr, &pBuf1, &dBuf1Stride, &pBuf2, &dBuf2Stride);

	if(dBuf1Stride == -1)
	{
		dBuf1Stride = FrmData->nStride;
	}
	if(dBuf2Stride == -1)
	{
		dBuf2Stride = FrmData->nStride;
	}
   // MMS_IL_PRINT("eDomain %d \n", portData->tPortDef.eDomain);
    if(dBuf1Stride == 0)
    {
        szPtrBuf1 = pBufHdr->nFilledLen;
        szPtrBuf2 = 0;
        dBuf2Stride = 0;
     //   MMS_IL_PRINT("szPtrBuf1 %d, szPtrBuf2 %d, dBuf1Stride %d, dBuf2Stride %d \n", szPtrBuf1, szPtrBuf2, dBuf1Stride, dBuf2Stride);
    }

    void* tmpBuff = malloc(szPtrBuf1 + szPtrBuf2);
    char* tmpBuffWalker = (char*)tmpBuff;

	MMS_IL_PRINT("pBuf1 0x%08X with stride %d\n", (unsigned int)pBuf1, dBuf1Stride);
	MMS_IL_PRINT("pBuf2 0x%08X with stride %d\n", (unsigned int)pBuf2, dBuf2Stride);
   // MMS_IL_PRINT("nAllocLen %d, nFilledLen %d\n", pBufHdr->nAllocLen, pBufHdr->nFilledLen);
    //MMS_IL_PRINT("pBufHdr->nOffset %lu, szPtrBuf1 %d\n", pBufHdr->nOffset, szPtrBuf1);

    dataPtr = (unsigned char*)(pBuf1) + pBufHdr->nOffset;
    unsigned int writtenBytes = 0;

    if (dBuf1Stride == 0)
    {//if no border available, applicable for jpeg saving
     //   MMS_IL_PRINT("dataPtr %p, tmpBuffWalker %p, szPtrBuf1 %d\n", dataPtr, tmpBuffWalker, szPtrBuf1);
        memcpy(tmpBuffWalker, dataPtr, szPtrBuf1);
    } else
    {
        int nWidth = FrmData->nBpp*FrmData->nWidth;
        while (writtenBytes < szPtrBuf1)
        {
           // MMS_IL_PRINT("dataPtr %p, tmpBuffWalker %p, nWidth %d\n", dataPtr, tmpBuffWalker, nWidth);
            memcpy(tmpBuffWalker, dataPtr, nWidth);
            tmpBuffWalker += nWidth;

            dataPtr += dBuf1Stride;
            writtenBytes += nWidth;
        }
    }

    int nOffset = pBufHdr->nOffset;

    if (dBuf2Stride > 0)
    {
        int lines = pBufHdr->nOffset/dBuf2Stride;
        int cols = pBufHdr->nOffset%dBuf2Stride;
        //MMS_IL_PRINT("lines %d, cols %d \n", lines, cols);
        nOffset = (lines>>1)*dBuf2Stride + cols;
        //MMS_IL_PRINT("nOffset %lu, szPtrBuf2 %d\n", lines, szPtrBuf2);

    }

    dataPtr = (unsigned char*)(pBuf2) + nOffset;
    writtenBytes = 0;

    if (dBuf2Stride == 0)
    { //if no border available, applicable for jpeg saving
      //  MMS_IL_PRINT("dataPtr %p, tmpBuffWalker %p, szPtrBuf1 %d\n", dataPtr, tmpBuffWalker, szPtrBuf1);
        memcpy(tmpBuffWalker, dataPtr, szPtrBuf2);
    } else
    {
        int nWidth = FrmData->nBpp*FrmData->nWidth;
      //   MMS_IL_PRINT("szPtrBuf2 %d\n",szPtrBuf2);
        while (writtenBytes < szPtrBuf2)
        {
            //MMS_IL_PRINT("dataPtr %p, tmpBuffWalker %p, nWidth %d\n", dataPtr, tmpBuffWalker, nWidth);
            memcpy(tmpBuffWalker, dataPtr, nWidth);
            tmpBuffWalker += nWidth;

            dataPtr += dBuf2Stride;
            writtenBytes += nWidth;
            //MMS_IL_PRINT("writtenBytes %d\n", writtenBytes);
        }
    }

    FrmData->nFrmCount++;
    OMX_OTHER_EXTRADATATYPE *extraData = pComp->getExtradata(pBufHdr->pPlatformPrivate, (OMX_EXTRADATATYPE)OMX_AncillaryData);
    if (extraData)
    {
        OMX_TI_ANCILLARYDATATYPE* ancilliary = (OMX_TI_ANCILLARYDATATYPE*)extraData->data;
        MMS_IL_PRINT("CameraView %d, snap %d\n", ancilliary->eCameraView, bIsSnap);

        if (ancilliary->eCameraView == OMX_2D_Snap && !bIsSnap)
        {
            strcpy(oldname, ServiceHeaderSaveBuffer.namePrfx);
            sprintf(ServiceHeaderSaveBuffer.namePrfx, "%s_(SNAP)", ServiceHeaderSaveBuffer.namePrfx);
            bIsSnap = true;
        } else if (ancilliary->eCameraView == OMX_2D_Prv && bIsSnap)
        {
            memset(ServiceHeaderSaveBuffer.namePrfx, NULL, 128);
            strcpy(ServiceHeaderSaveBuffer.namePrfx, oldname);
            bIsSnap = false;
        }
    }
    // MMS_IL_PRINT("Going to SaveBuffer.Init\n");
    //MMS_IL_PRINT("XXXXXXXXXnamePrfx %s\n", ServiceHeaderSaveBuffer.namePrfx);
    //MMS_IL_PRINT("XXXXXXXXXnamePrfx %s\n", ServiceHeaderSaveBuffer.fileExt);
    SaveBuffer->Init(pComp,  associatedPort, szPtrBuf1 + szPtrBuf2, &ServiceHeaderSaveBuffer);
   // MMS_IL_PRINT("SaveBuffer.Init passed\n");
    SaveBuffer->Transfer(tmpBuff);
   // MMS_IL_PRINT("SaveBuffer.Transfer passed\n");
    free(tmpBuff);
	StopExecute();

    return TIMM_OSAL_ERR_NONE;
}