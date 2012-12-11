#include "inc/PortService/Implementation/COMXPortServiceMetadOIP.h"

TIMM_OSAL_ERRORTYPE COMXPortServiceMetadOIP::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
		return err;
	}

    outDesc = (dataTransferDescT*)malloc(sizeof(dataTransferDescT));

    if (outDesc == NULL)
	{
        printf("MetadOIP: service initilization failed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    outDesc->initiatorID = pComp->nComponentId;
    outDesc->dataStride = 0;
    outDesc->dataTypeID = 0;
    outDesc->transferMsgIdD = 0;
    outDesc->tranfMedia = DTM_NETWORK;
    outDesc->customData = NULL;
    outDesc->OnTransferDone = NULL;

    // connect to socket
    int mysocket;
    struct sockaddr_in pc;

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mysocket == -1)
	{
        printf("Metadata over IP: Cant get a socket!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    char ipaddr[] = META_OVER_IP_DEFAULT_ADDR;
    int ipaddrlen = 0;
    short int port = META_OVER_IP_DEFAULT_PORT;
    FILE *config_file = NULL;

    config_file = fopen(META_OVER_IP_CONFIG_FILE, "r");
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

    if (connect(mysocket, (struct sockaddr *)&pc, sizeof(struct sockaddr)) != 0)
	{
        printf("Metadata over IP: Cant connect socket! Error str: %s\n", strerror(errno));
        printf("Tried IP ADDR: Len[%d], Content[%s]\n", strlen(ipaddr), ipaddr);
        printf("Tried PORT: %hd\n", port);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    printf("Metadata over IP: Socket connected!\n");
    //Message id is used as the socket storage
    outDesc->transferMsgIdD = mysocket;

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceMetadOIP::Deinit()
{
    if(outDesc == NULL)
    {
        printf("Meta data over ip: service not initialized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    //Message id is used as the socket storage
    // Close socket and set to 0
    close(outDesc->transferMsgIdD);
    outDesc->transferMsgIdD = 0;
    free(outDesc);
	outDesc = NULL;

    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceMetadOIP::ConfigPortService()
{
    return TIMM_OSAL_ERR_NONE;

}


TIMM_OSAL_ERRORTYPE COMXPortServiceMetadOIP::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
     eAllocType = eAllocType; //stop warning: unused parameter

	if(outDesc == NULL)
    {
        printf("MetadOIP: service not initilized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    OMX_TI_PLATFORMPRIVATE* pvtData = (OMX_TI_PLATFORMPRIVATE*)(pBufHdr->pPlatformPrivate);

    if(pvtData->nMetaDataSize > 0 && pvtData->pMetaDataBuffer != NULL)
    {

        if(outDesc == NULL)
        {
            printf("Meta data over ip: service not initialized!\n");
			StopExecute();
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        OMX_OTHER_EXTRADATATYPE *extData = (OMX_OTHER_EXTRADATATYPE *) pvtData->pMetaDataBuffer;
        while ((unsigned char *) extData < (unsigned char *) pvtData->pMetaDataBuffer + pvtData->nMetaDataSize && extData->eType != OMX_ExtraDataNone) {
            extData = (OMX_OTHER_EXTRADATATYPE *) ((unsigned char *) extData + extData->nSize);
        }

        outDesc->initiatorPort = pBufHdr->nOutputPortIndex;
// #ifndef OMX_SKIP64BIT
        // sprintf((char *) outDesc->namePrfx, "md_ts%lld__",
                // (OMX_TICKS) pBufHdr->nTimeStamp);
// #else
        // sprintf((char *) outDesc->namePrfx, "md_ts%lu%lu__",
                // (OMX_U32) pBufHdr->nTimeStamp.nHighPart,
                // (OMX_U32) pBufHdr->nTimeStamp.nLowPart);
// #endif
        sprintf((char *) outDesc->namePrfx, "metadata__");
        strcpy((char *) outDesc->fileExt, "bin");
        outDesc->dataBuffPtr = (unsigned char*)pvtData->pMetaDataBuffer;
        outDesc->dataOffset = 0;
        outDesc->dataSize = (unsigned int) extData - (unsigned int) pvtData->pMetaDataBuffer;

        //Message id is used as the socket storage
        if (outDesc->transferMsgIdD)
        {
            if ( -1 == send(outDesc->transferMsgIdD, pvtData->pMetaDataBuffer, outDesc->dataSize, 0))
                perror("\nSEND: \n");
        }
    }

	StopExecute();
    return TIMM_OSAL_ERR_NONE;
}
