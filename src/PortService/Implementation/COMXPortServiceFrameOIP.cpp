#include "inc/PortService/Implementation/COMXPortServiceFrameOIP.h"

void* frameOverIP_thread_function(void *arg)
{
	FrmOvrIPData_t *prvOvrIPData = (FrmOvrIPData_t *)arg;
    unsigned int bytesSent = 0;

	printf("\nThread created\n");
	while (1)
	{
        sem_post(&prvOvrIPData->semThreadFree);
                sem_wait(&prvOvrIPData->semSend);
		if (0 == sem_trywait(&prvOvrIPData->semExit))
		{
			printf("\nThread EXITING\n");
			pthread_exit((void*)("Thread exited"));
		}

        bytesSent = send(prvOvrIPData->nSocket, prvOvrIPData->thBuff, (size_t)prvOvrIPData->nBuffSize, 0);
        MMS_IL_PRINT("\n\n\n bytesSent = %d \n\n", bytesSent);
        if (bytesSent != prvOvrIPData->nBuffSize)
        {
			pthread_exit((void*)("VBShit"));
        }
        prvOvrIPData->nNumFramesSent++;
	}

    return NULL;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameOIP::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	COMXPortHandle* portData;
	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
		return err;
	}

    TIMM_OSAL_S32 mysocket;
    struct sockaddr_in pc;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mysocket == -1)
	{
        printf("\nFrame over IP: Cant get a socket!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    char ipaddr[] = FRAME_OVER_IP_DEFAULT_ADDR;
    int ipaddrlen = 0;
    short int port = FRAME_OVER_IP_DEFAULT_PORT;
    FILE* config_file = NULL;

    config_file = fopen(FRAME_OVER_IP_CONFIG_FILE, "r");
    if (NULL != config_file)
	{
        memset((void*)ipaddr, 0, sizeof(ipaddr));
        fgets(ipaddr, 17, config_file);
        ipaddrlen = strlen(ipaddr);
        ipaddr[ipaddrlen-1] = '\0';
        if (ipaddr[ipaddrlen-2] == '\r')
		{
            ipaddr[ipaddrlen-2] = '\0';
		}
        fscanf(config_file, "%hu", &port);
        fclose(config_file);
    }

    memset(&pc, 0, sizeof(pc));
    pc.sin_family = AF_INET;
    pc.sin_addr.s_addr = inet_addr(ipaddr);
    pc.sin_port = htons(port);

    if(connect(mysocket, (struct sockaddr *)&pc, sizeof(struct sockaddr)) != 0)
	{
        printf("\nFrame over IP: Can't connect socket! Error str: %s\n", strerror(errno));
        printf("Tried IP ADDR: Len[%d], Content[%s]\n", strlen(ipaddr), ipaddr);
        printf("Tried PORT: %hd\n", port);
        return TIMM_OSAL_ERR_PARAMETER;
    }
    printf("Frame over IP: Socket connected!\n");
    prvOvrIPData = (FrmOvrIPData_t*)malloc(sizeof(FrmOvrIPData_t));
    if(prvOvrIPData == NULL)
	{
        return TIMM_OSAL_ERR_ALLOC;
    }

	portData = pComp->GetPortData(associatedPort);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data\n", associatedPort);
		return TIMM_OSAL_ERR_OMX;
	}

    if(portData->tPortDef.eDomain == OMX_PortDomainVideo)
	{
        prvOvrIPData->nStride = portData->tPortDef.format.video.nStride;
        prvOvrIPData->nWidth = portData->tPortDef.format.video.nFrameWidth;
        prvOvrIPData->nHeight = portData->tPortDef.format.video.nFrameHeight;
        if (portData->tPortDef.format.video.eColorFormat == OMX_COLOR_FormatCbYCrY)
		{
            prvOvrIPData->nBpp = 2;
        }
		else if (portData->tPortDef.format.video.eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar || portData->tPortDef.format.video.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
		{
            prvOvrIPData->nBpp = 1;
        }
    }
	else if(portData->tPortDef.eDomain == OMX_PortDomainImage)
	{
        prvOvrIPData->nStride = portData->tPortDef.format.image.nStride;
        prvOvrIPData->nWidth = portData->tPortDef.format.image.nFrameWidth;
        prvOvrIPData->nHeight = portData->tPortDef.format.image.nFrameHeight;
        if (portData->tPortDef.format.image.eColorFormat == OMX_COLOR_FormatCbYCrY)
		{
            prvOvrIPData->nBpp = 2;
        }
		else if (portData->tPortDef.format.image.eColorFormat == OMX_COLOR_FormatYUV420SemiPlanar || portData->tPortDef.format.image.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
		{
            prvOvrIPData->nBpp = 1;
        }
    }
	else
	{
        printf("Preview frame capture: Unsupported port domain!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    prvOvrIPData->nSocket = mysocket;
    prvOvrIPData->thBuff = (char*)malloc(4*1024*1024);
    sem_init(&prvOvrIPData->semSend, 0, 0);
    sem_init(&prvOvrIPData->semExit, 0, 0);
    sem_init(&prvOvrIPData->semThreadFree, 0, 0);
    if(pthread_create(&prvOvrIPData->FrameSendThread, NULL, frameOverIP_thread_function, (void*)prvOvrIPData))
	{
        perror("Thread creation failed");
        return TIMM_OSAL_ERR_UNKNOWN;
     }

    printf("Thread init message !\n");
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameOIP::Deinit()
{
    int res;
    void *thread_result;

	if(prvOvrIPData == NULL)
	{
		printf("Frame over ip: service not initialized!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    sem_post(&prvOvrIPData->semExit);
    sem_post(&prvOvrIPData->semSend);
    res = pthread_join(prvOvrIPData->FrameSendThread, &thread_result);
    if (res != 0)
	{
	    perror("Thread join failed\n");
	    return TIMM_OSAL_ERR_UNKNOWN;
    }
    free(prvOvrIPData->thBuff);
    sem_destroy(&prvOvrIPData->semSend);
    sem_destroy(&prvOvrIPData->semExit);
    sem_destroy(&prvOvrIPData->semThreadFree);
    close(prvOvrIPData->nSocket);
    prvOvrIPData->nSocket = 0;
	free(prvOvrIPData);
	prvOvrIPData = NULL;

    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameOIP::ConfigPortService()
{
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameOIP::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
     eAllocType = eAllocType; //stop warning: unused parameter

	 if(!StartExecute())
	{
        printf("FrameOIP: service not STARTED!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    OMX_TI_PLATFORMPRIVATE* pvtData = (OMX_TI_PLATFORMPRIVATE*)(pBufHdr->pPlatformPrivate);

    if (pBufHdr->nFilledLen <= 0 || pBufHdr->pBuffer == NULL)
	{
		StopExecute();
         return TIMM_OSAL_ERR_UNKNOWN;
	}

    if (NULL == prvOvrIPData)
	{
        printf("FrameOIP: service not initialized!\n");
		StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
	}

    prvOvrIPData->nNumFrames++;

    if (0 != sem_trywait(&prvOvrIPData->semThreadFree))
	{
		StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    unsigned int linesz = prvOvrIPData->nWidth*prvOvrIPData->nBpp;
    unsigned char* dataPtr = (unsigned char*)(pBufHdr->pBuffer) + pBufHdr->nOffset;
    unsigned char* tmpPtr = (unsigned char*)(pBufHdr->pBuffer) + pBufHdr->nOffset;
    unsigned char* szPtr = dataPtr + pBufHdr->nFilledLen;

    if(prvOvrIPData->nSocket == 0)
	{
		StopExecute();
         return TIMM_OSAL_ERR_UNKNOWN;
    }

    PrvOutgoingFrmHdr_t frmDesc;
    frmDesc.timeStamp = pBufHdr->nTimeStamp;
    frmDesc.validDataOffset = 0;//pBufHdr->nOffset;

    frmDesc.frameSize = 0;
    while (tmpPtr < szPtr)
	{
        frmDesc.frameSize += linesz;
        tmpPtr += prvOvrIPData->nStride;
    }

    //frmDesc.frameSize = (pBufHdr->nFilledLen/prvOvrIPData->nStride)*linesz;//pBufHdr->nFilledLen;
    frmDesc.byteSize = frmDesc.frameSize;

    {
        OMX_OTHER_EXTRADATATYPE *extData = (OMX_OTHER_EXTRADATATYPE *) pvtData->pMetaDataBuffer;
        frmDesc.metaSize = 0;

        while ( (unsigned int) extData < (unsigned int) pvtData->pMetaDataBuffer + pvtData->nMetaDataSize && extData->eType != OMX_ExtraDataNone)
		{
//            while ( extData->eType != OMX_ExtraDataNone ) {
            extData = (OMX_OTHER_EXTRADATATYPE *) ((unsigned int) extData->data + extData->nDataSize);
            frmDesc.metaSize += extData->nDataSize;
        }
        frmDesc.validMetaOffset = frmDesc.validDataOffset + frmDesc.frameSize;
//        frmDesc.metaSize = (unsigned int) extData - (unsigned int) pvtData->pMetaDataBuffer;
        frmDesc.byteSize += frmDesc.metaSize;
    }

	int offset = 0;
	memcpy((void*) (prvOvrIPData->thBuff + offset), (void*)&frmDesc, sizeof(PrvOutgoingFrmHdr_t));
	offset += sizeof(PrvOutgoingFrmHdr_t);

    while (dataPtr < szPtr)
	{
        memcpy((void*) (prvOvrIPData->thBuff + offset), dataPtr, linesz);
        offset += linesz;
        dataPtr += prvOvrIPData->nStride;
    }

    if( frmDesc.metaSize > 0)
	{
        memcpy((void*) (prvOvrIPData->thBuff + offset), (void*)pvtData->pMetaDataBuffer, frmDesc.metaSize);
        offset += frmDesc.metaSize;
    }

    prvOvrIPData->nBuffSize = offset;
    sem_post(&prvOvrIPData->semSend);

    framePrintOut++;
    if ( 0 == (framePrintOut % 25) )
	{
        printf("Frames sent %d, all: %d\n", prvOvrIPData->nNumFramesSent, prvOvrIPData->nNumFrames);
	}

	StopExecute();
    return TIMM_OSAL_ERR_NONE;
}
