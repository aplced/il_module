#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
#include "inc/lib_comm.h"
#include <queue>
//#define MMS_IL_PRINT(ARGS...)
OMX_ERRORTYPE OMXComponentEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
OMX_ERRORTYPE OMXComponentFillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);
OMX_ERRORTYPE OMXComponentEmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

COMXComponent::COMXComponent(COMXComponentDispatcher* hDispatcher, char* iCompName)
{
    compName = (char*)malloc(strlen(iCompName) + 1);
    strcpy(compName, iCompName);

    //Init the dispatcher
    dispatcher = hDispatcher;
    //Init pthread mutex for pComponentEvents
    TIMM_OSAL_EventCreate(&pComponentEvents);

    ComponentState = OMX_StateLoaded;
    LastStateCommand = OMX_StateLoaded;

    //Init omx_comp
    buffMgr = new COMXBuffDoneManager(this);
    //Init callbacks
    pComponentCallbacks = (OMX_CALLBACKTYPE*)malloc(sizeof(OMX_CALLBACKTYPE));
    pComponentCallbacks->EventHandler = OMXComponentEventHandler;
    pComponentCallbacks->EmptyBufferDone = OMXComponentEmptyBufferDone;
    pComponentCallbacks->FillBufferDone = OMXComponentFillBufferDone;
    displayModule = NULL;
    nMemMarshallingListSz = 0;
    pMemMarshalling = NULL;
    tMemMarshallingAllocStrat = MEM_ION_1D;
    portHdls.clear();
    portServices.clear();
    sem_init(&portServiceMutex, 0, 1);
    sem_init(&get_port_sem, 0, 1);
}

OMX_ERRORTYPE COMXComponent::GetOMXCompHdl()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    MMS_IL_PRINT("OMX_GetHandle @ %s with ID %d\n", compName, nComponentId);
    error = OMX_GetHandle(&pCompHandle, compName, this, pComponentCallbacks);
    if (OMX_ErrorNone != error)
    {
        MMS_IL_PRINT("GetHandle Error %08X\n", error);
        return error;
    }

    InitializePortData();

    return OMX_ErrorNone;
}

COMXComponent::~COMXComponent()
{
    TIMM_OSAL_EventDelete(pComponentEvents);

    DeinitializePortData();

    sem_destroy(&portServiceMutex);
    sem_destroy(&get_port_sem);
    if(buffMgr != NULL)
    {
        delete(buffMgr);
    }

    if(compName != NULL)
    {
        delete(compName);
    }

    if (OMX_ErrorNone != OMX_FreeHandle(pCompHandle))
    {
        MMS_IL_PRINT("FreeHandle Error TIMM_OSAL_ERR_UNKNOWN\n");
    }
}

OMX_ERRORTYPE COMXComponent::GetComponentVersion(OMX_VERSIONTYPE* compVersion, OMX_VERSIONTYPE* specVersion, OMX_UUIDTYPE* copmUUID)
{
    return OMX_GetComponentVersion(pCompHandle, (OMX_STRING) compName, compVersion, specVersion, copmUUID);
}

OMX_ERRORTYPE COMXComponent::SetParam(OMX_INDEXTYPE structId, unsigned char* dataPtr)
{
    OMX_ERRORTYPE eError = OMX_SetParameter(pCompHandle, structId, dataPtr);
    if(eError != OMX_ErrorNone)
        MMS_IL_PRINT("OMX_SetParameter with error 0x%08x for structure id 0x%08x\n", eError, structId);

    return eError;
}

OMX_ERRORTYPE COMXComponent::GetParam(OMX_INDEXTYPE structId, unsigned char* dataPtr)
{
    OMX_ERRORTYPE eError = OMX_GetParameter(pCompHandle, structId, dataPtr);
    if(eError != OMX_ErrorNone)
        MMS_IL_PRINT("OMX_GetParameter with error 0x%08x for structure id 0x%08x\n", eError, structId);

    return eError;
}

OMX_ERRORTYPE COMXComponent::SetConfig(OMX_INDEXTYPE structId, unsigned char* dataPtr)
{
    OMX_ERRORTYPE eError = OMX_SetConfig(pCompHandle, structId, dataPtr);
    if(eError != OMX_ErrorNone)
        MMS_IL_PRINT("OMX_SetConfig with error 0x%08x for structure id 0x%08x\n", eError, structId);

    return eError;
}

OMX_ERRORTYPE COMXComponent::GetConfig(OMX_INDEXTYPE structId, unsigned char* dataPtr)
{
    OMX_ERRORTYPE eError = OMX_GetConfig(pCompHandle, structId, dataPtr);
    if(eError != OMX_ErrorNone)
        MMS_IL_PRINT("OMX_GetConfig with error 0x%08x for structure id 0x%08x\n", eError, structId);

    return eError;
}

OMX_ERRORTYPE COMXComponent::Command(void* pCommandMessage, void* pComponentData)
{
    OMX_ERRORTYPE retval = OMX_ErrorNone;
    COMXComponent* pComp = (COMXComponent*)pComponentData;
    commandMessage_t* commandMessage = (commandMessage_t*)pCommandMessage;
    MMS_IL_PRINT("Sending command to OMX Component %d\n", nComponentId);

    if (commandMessage->nCommand == OMX_CommandStateSet)
    {
        sem_init(&state_set_sem, 0, 1);
        MMS_IL_PRINT("OMX_CommandStateSet\n");
        if (ComponentState == (OMX_STATETYPE)(commandMessage->nParam))
        {
            return retval;
        }
        switch (commandMessage->nParam)
        {
            case OMX_StateLoaded:
                MMS_IL_PRINT("Setting to LOADED\n");
                retval = SetStateLoaded();
                break;
            case OMX_StateIdle:
                MMS_IL_PRINT("Setting to IDLE\n");
                retval = SetStateIdle();
                break;
            case OMX_StateExecuting:
                MMS_IL_PRINT("Setting to EXECUTING\n");
                retval = SetStateExecuting();
                break;
            default:
                retval = OMX_SendCommand(pCompHandle, OMX_CommandStateSet, commandMessage->nParam, NULL);
                break;
        }
        LastStateCommand = (OMX_STATETYPE)commandMessage->nParam;
        sem_destroy(&state_set_sem);
    }
    else if (commandMessage->nCommand == OMX_CommandPortDisable)
    {
        MMS_IL_PRINT("OMX_CommandPortDisable\n");
        retval = PortDisable(pComp, commandMessage->nParam);
    }
    else if (commandMessage->nCommand == OMX_CommandPortEnable)
    {
        MMS_IL_PRINT("OMX_CommandPortEnable\n");
        retval = PortEnable(commandMessage->nParam);
    }
    else
    {
        MMS_IL_PRINT("GENERIC\n");
        retval = OMX_SendCommand(pCompHandle, (OMX_COMMANDTYPE)commandMessage->nCommand, commandMessage->nParam, NULL);
    }

    return retval;
}

OMX_ERRORTYPE COMXComponent::PortDisable(void* pComponent, unsigned int nPort)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXComponent* pComp = (COMXComponent*)pComponent;

    eError = OMX_SendCommand(pCompHandle, OMX_CommandPortDisable, nPort, NULL);
    if (eError != OMX_ErrorNone)
        return eError;

    eError = WaitForPortDisable();
    if (eError != OMX_ErrorNone)
        return eError;

    if (nPort == OMX_ALL)
    {
        eError = FreeAllPortBuffers();
    }
    else
    {
        eError = FreePortBuffers(nPort);
    }
    if (eError != OMX_ErrorNone)
        return eError;

    return eError;
}

OMX_ERRORTYPE COMXComponent::PortEnable(unsigned int nPort)
{
    OMX_ERRORTYPE eError = OMX_SendCommand(pCompHandle, OMX_CommandPortEnable, nPort, NULL);
    if (eError != OMX_ErrorNone)
    {
        return eError;
    }

    return eError;
}

OMX_ERRORTYPE OMXComponentEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
    COMXComponent* pComponentEntry = (COMXComponent*)ptrAppData;
    hComponent = hComponent;
    pEventData = pEventData;
    MMS_IL_PRINT("OMX Component %d generated event %d\n", pComponentEntry->nComponentId, eEvent);

    if (!ptrAppData)
    {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    switch (eEvent)
    {
        case OMX_EventCmdComplete:
            if (nData1 == OMX_CommandStateSet)
            {
                MMS_IL_PRINT("Event - OMX_CommandStateSet complete on comp %d\n", pComponentEntry->nComponentId);
                sem_post(&(pComponentEntry->state_set_sem));
                //retval = TIMM_OSAL_EventSet(pComponentEntry->pComponentEvents, COMPONENT_EVENT_CMD_STATE_CHANGE, TIMM_OSAL_EVENT_OR);

                //if(retval != TIMM_OSAL_ERR_NONE)
               // {
                 //   eError = OMX_ErrorBadParameter;
                  //  MMS_IL_PRINT("TIMM_OSAL_EventSet failed with 0x%08X\n", retval);
               // }
            }
            else if (nData1 == OMX_CommandPortDisable)
            {
                MMS_IL_PRINT("Event - OMX_CommandPortDisable completeon comp %d\n", pComponentEntry->nComponentId);

                retval = TIMM_OSAL_EventSet(pComponentEntry->pComponentEvents, COMPONENT_EVENT_CMD_PORT_DISABLE, TIMM_OSAL_EVENT_OR);

                if(retval != TIMM_OSAL_ERR_NONE)
                {
                    eError = OMX_ErrorBadParameter;
                    MMS_IL_PRINT("TIMM_OSAL_EventSet failed with 0x%08X\n", retval);
                }
            }
            else if (nData1 == OMX_CommandPortEnable)
            {
                MMS_IL_PRINT("Event - OMX_CommandPortEnable completeon comp %d\n", pComponentEntry->nComponentId);
                retval = TIMM_OSAL_ERR_NONE;
                // retval =
                    // TIMM_OSAL_EventSet(pComponentEntry->pComponentEvents,
                    // COMPONENT_EVENT_CMD_PORT_ENABLE, TIMM_OSAL_EVENT_OR);
                // GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),
                                                // OMX_ErrorBadParameter);
            }
            break;
        case OMX_EventError:
            MMS_IL_PRINT("Event - OMX_EventError: nData1 = 0x%08X, nData2 = 0x%08X on comp %d\n", nData1, nData2, pComponentEntry->nComponentId);
            break;
        case OMX_EventBufferFlag:
            MMS_IL_PRINT("Event - OMX_EventBufferFlag: Input port = %lu, Buffer flags = 0x%08lx on comp %d\n", nData1, nData2, pComponentEntry->nComponentId);
            break;
        case OMX_EventIndexSettingChanged:
            MMS_IL_PRINT("Event - OMX_EventIndexSettingChanged: nData1 = %lu, nData2 = %lu on comp %d\n", nData1, nData2, pComponentEntry->nComponentId);
            break;
        case OMX_EventMark:
        case OMX_EventPortSettingsChanged:
        case OMX_EventResourcesAcquired:
        case OMX_EventComponentResumed:
        case OMX_EventDynamicResourcesAvailable:
        case OMX_EventPortFormatDetected:
        case OMX_EventMax:
            MMS_IL_PRINT("OMX not implemented event: eEvent = %u, nData1 = %lu, nData2 = %lu  on comp %d", eEvent, nData1, nData2, pComponentEntry->nComponentId);
            eError = OMX_ErrorNotImplemented;
            break;
        default:
            MMS_IL_PRINT("OMX_ErrorBadParameter\n");
            eError = OMX_ErrorBadParameter;
            break;
    }

EXIT:
    return eError;
}

OMX_ERRORTYPE OMXComponentFillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer)
{
    COMXComponent* pComponentEntry = (COMXComponent*)ptrAppData;
    systemMessage_t message;
    int nPortIndex;
    // We are in a process of stopping - ignore BufferDone callbacks
    if (pComponentEntry->buffMgr->ThreadExit())
    {
        return OMX_ErrorNone;
    }
    nPortIndex = pBuffer->nOutputPortIndex;
    MMS_IL_PRINT("Fill buffer done on OMX Component %d port number %d\n", pComponentEntry->nComponentId, nPortIndex);


    if (nPortIndex == PREVIEW_PORT || nPortIndex == CAPTURE_PORT)
    {
        message.nId = ((nPortIndex == CAPTURE_PORT)?OMX_FILL_BUFFER_CAPTURE:OMX_FILL_BUFFER_PREVIEW);
        message.nStatus = 0;
        message.nPayloadSize = 4;
        int nPlayload = pComponentEntry->nComponentId;
        message.pPayload = &nPlayload;
        //SEND TO AL
        pComponentEntry->dispatcher->SendMessage(&message);
    }
    if (pComponentEntry->ComponentState != OMX_StateExecuting)
    {
        MMS_IL_PRINT("OMX Component not in Executing state - exiting with nothing to do\n");
        return OMX_ErrorNone;
    }
    //MMS_IL_PRINT("Push pBuffer - 0x%08X\n", pBuffer);
    pComponentEntry->buffMgr->Push(OMX_FILL, pBuffer);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXComponentEmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer)
{
    COMXComponent* pComponentEntry = (COMXComponent*)ptrAppData;
    systemMessage_t message;
    //COMXPortHandle* portData = pComponentEntry->GetPortData(pBuffer->nInputPortIndex);
    // We are in a process of stopping - ignore BufferDone callbacks
    if (pComponentEntry->buffMgr->ThreadExit())
    {
        return OMX_ErrorNone;
    }

    MMS_IL_PRINT("Empty buffer done on OMX Component %d port number %lu\n", pComponentEntry->nComponentId, pBuffer->nInputPortIndex);
    message.nId = OMX_EMPTY_BUFFER_DONE;
    message.nStatus = 0;
    message.nPayloadSize = 4;
    int nPlayload = pComponentEntry->nComponentId;
    message.pPayload = &nPlayload;
    //SEND TO AL
    pComponentEntry->dispatcher->SendMessage(&message);

    if (pComponentEntry->ComponentState != OMX_StateExecuting)
    {
        MMS_IL_PRINT("OMX Component not in Executing state - exiting with nothing to do\n");
        return OMX_ErrorNone;
    }

   /* for (int nBuffIdx = 0; nBuffIdx < portData->nBufferCount; nBuffIdx ++)
    {
        if (portData->pComponentBuffers[nBuffIdx].pBuffer == pBuffer)
        {
            portData->pComponentBuffers[nBuffIdx].bFree = TIMM_OSAL_TRUE;
            break;
        }
    }*/
    //pComponentEntry->buffMgr->Push(OMX_EMPTY, pBuffer);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXComponent::AddPortSevice(int portNum, int serviceType, void *pServiceHeader, int nServiceHeadSize)
{
    COMXPortService* service = NULL;
    COMXPortHandle* portData = GetPortData(portNum);

    if (portData == NULL)
    {
        MMS_IL_PRINT("Failed to identify port index in OMX component ports\n");
        return OMX_ErrorBadParameter;
    }

    service = dispatcher->portServices.CreatePortService(serviceType);
    if(service == NULL)
    {
        MMS_IL_PRINT("Failed to identify requested service\n");
        return OMX_ErrorBadParameter;
    }
    service->Init(this, portNum, pServiceHeader, nServiceHeadSize);
    service->Enable();

    sem_wait(&portServiceMutex);

    MMS_IL_PRINT(" add service %d on comp %d\n", service->ServiceId(), nComponentId);
    portServices.push_back(service);

    sem_post(&portServiceMutex);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXComponent::RemovePortService(unsigned int portNum, int serviceType)
{
    COMXPortService* service = NULL;

    sem_wait(&portServiceMutex);
    int i, end = portServices.size();

    for(i = 0; i < end; i++)
    {
        if(portServices[i]->ServiceId() == serviceType &&
           portServices[i]->Port()      == portNum)
        {
            service = portServices[i];
            portServices[i]->Disable();
            portServices[i]->Deinit();
            MMS_IL_PRINT("Remove service %d from comp %d\n", portServices[i]->ServiceId(), nComponentId);
            portServices.erase(portServices.begin() + i);
           // MMS_IL_PRINT("Port service %d is erased\n", i);
            delete(service);
            service = NULL;
            break;
        }
    }

    if(i == end)
    {
        sem_post(&portServiceMutex);
        MMS_IL_PRINT("Failed to identify requested service\n");
        return OMX_ErrorBadParameter;
    }

    sem_post(&portServiceMutex);

    return OMX_ErrorNone;
}

void COMXComponent::RunPortServices(COMXPortHandle* pCompPort, OMX_BUFFERHEADERTYPE* pBufHdr)
{
    queue<COMXPortService*> service;

    sem_wait(&portServiceMutex);

    for(unsigned int i = 0; i < portServices.size(); i++)
    {
        if(portServices[i]->Port() == pCompPort->nPortNo)
        {//find all services for the given port and add them to service queue
            MMS_IL_PRINT(" Run portService[%d] %d on comp %d\n", i, portServices[i]->ServiceId(), nComponentId);
            service.push(portServices[i]);
        }
    }

    sem_post(&portServiceMutex);
    //execute all services for the given port
    while(!service.empty())
    {
        service.front()->Execute(pBufHdr, pCompPort->eAllocType);
        //MMS_IL_PRINT("Port Service is executed!\n");
        service.pop();
        //pop function is void and does not return the deleted element
    }

}

void COMXComponent::ConfigPortServices(configPortService_t* pConfig, int size)
{
    sem_wait(&portServiceMutex);
    int service_num = pConfig->nServiceNumber;

    for(unsigned int i = 0; i < portServices.size(); i++)
    {
        if(portServices[i]->Port() == pConfig->nPortNumber && portServices[i]->ServiceId() == service_num)
        {
           portServices[i]->PopulateConfig(pConfig->pConfig, size);
           MMS_IL_PRINT("Config of portServices[%d] %d of component %d is populated!\n", i, portServices[i]->ServiceId(), nComponentId);
           if(service_num != SAVE_BUFFER_SRVC)
           {
               portServices[i]->ConfigPortService();
               portServices[i]->PopulateConfig(NULL, 0);
           }
           break;
        }
    }

    sem_post(&portServiceMutex);
}

void COMXComponent::ConfigAllPortServices()
{
    sem_wait(&portServiceMutex);

    for(unsigned int i = 0; i < portServices.size(); i++)
    {
         portServices[i]->ConfigPortService();
         portServices[i]->PopulateConfig(NULL, 0);
    }

    sem_post(&portServiceMutex);
}

OMX_ERRORTYPE COMXComponent::GetOMXInitPortData(OMX_INDEXTYPE portInitIndx, int* portCount, int* portStartIndx)
{
    OMX_PORT_PARAM_TYPE tPortParams;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_INIT_STRUCT_PTR(&tPortParams, OMX_PORT_PARAM_TYPE);
    //Get port parameter for given component handle and given port
    eError = OMX_GetParameter(pCompHandle, portInitIndx, &tPortParams);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX_GetParameter OMX_IndexParam**PORT**Init failed with error %08X\n", eError);
        return eError;
    }

    MMS_IL_PRINT("OMX_IndexParam**PORT**Init port count %d port start index %d on comp %d\n", (int)tPortParams.nPorts, (int)tPortParams.nStartPortNumber, nComponentId);
    *portCount = tPortParams.nPorts;
    *portStartIndx = tPortParams.nStartPortNumber;

    MMS_IL_PRINT("GetOMXInitPortData returning with status 0x%08X\n", eError);
    return eError;
}

void COMXComponent::InitPortData(int numPorts, int startIndex)
{
    MMS_IL_PRINT("InitPortData port count %d port start index %d on comp %d\n", numPorts, startIndex, nComponentId);
    for (int idx = 0; idx < numPorts; idx++)
    {
        MMS_IL_PRINT("Adding Port handle %d\n", startIndex + idx);
        COMXPortHandle* portHdl = new COMXPortHandle();
        portHdls[startIndex + idx] = portHdl;
        portHdl->nPortNo = startIndex + idx;
        portHdl->nDisplayId = -1;
        portHdl->bAutoBufferCycle = true;
        portHdl->eAllocType = MEM_OMX;
        portHdl->bSupplierPort = true;
        portHdl->pPortCallback = NULL;
        portHdl->pLinkedComponent = NULL;
        portHdl->pLinkedPort = NULL;
        portHdl->pComponentBuffers = NULL;
        portHdl->nBufferCount = 0;

        OMX_INIT_STRUCT_PTR(&(portHdl->tPortDef), OMX_PARAM_PORTDEFINITIONTYPE);
        portHdl->tPortDef.nPortIndex = portHdl->nPortNo;
    }
}

OMX_ERRORTYPE COMXComponent::InitializePortData()
{
    MMS_IL_PRINT("Initializing video port data structures on comp %d\n", nComponentId);
    if (GetOMXInitPortData(OMX_IndexParamVideoInit, &videoPortCount, &videoPortStartIndex) == OMX_ErrorNone)
    {
        InitPortData(videoPortCount, videoPortStartIndex);
    }
    else
    {
        return OMX_ErrorUndefined;
    }

    MMS_IL_PRINT("Initializing audio port data structures\n");
    if (GetOMXInitPortData(OMX_IndexParamAudioInit, &audioPortCount, &audioPortStartIndex) == OMX_ErrorNone)
    {
        InitPortData(audioPortCount, audioPortStartIndex);
    }
    else
    {
        return OMX_ErrorUndefined;
    }

    MMS_IL_PRINT("Initializing image port data structures\n");
    if (GetOMXInitPortData(OMX_IndexParamImageInit, &imagePortCount, &imagePortStartIndex) == OMX_ErrorNone)
    {
        InitPortData(imagePortCount, imagePortStartIndex);
    }
    else
    {
        return OMX_ErrorUndefined;
    }

    MMS_IL_PRINT("Initializing other port data structures\n");
    if (GetOMXInitPortData(OMX_IndexParamOtherInit, &otherPortCount, &otherPortStartIndex) == OMX_ErrorNone)
    {
        InitPortData(otherPortCount, otherPortStartIndex);
    }
    else
    {
        return OMX_ErrorUndefined;
    }

    return OMX_ErrorNone;
}

void COMXComponent::ReleasePortResources(int numPorts, int startIndx)
{
//    int nPortIdx = 0;
    for(int i = startIndx; i < numPorts; i++)
    {
        delete(portHdls[i]);
    }
}

void COMXComponent::DeinitializePortData()
{
    MMS_IL_PRINT("Releasing component port data structures on comp %d\n", nComponentId);

    ReleasePortResources(videoPortCount, videoPortStartIndex);

    ReleasePortResources(audioPortCount, audioPortStartIndex);

    ReleasePortResources(imagePortCount, imagePortStartIndex);

    ReleasePortResources(otherPortCount, otherPortStartIndex);

    portHdls.clear();
}

COMXPortHandle* COMXComponent::GetPortData(OMX_U32 nPort)
{
    COMXPortHandle* pPortData;

    //MMS_IL_PRINT("Entering getCompPort\n");
    sem_wait(&get_port_sem);
    pPortData = portHdls[nPort];

    MMS_IL_PRINT("OMX_GetParameter OMX_IndexParamPortDefinition for port %d, %d of comp %d\n", (int)nPort, (int)pPortData->tPortDef.nPortIndex, nComponentId);
    // OMX_INIT_STRUCT_PTR(&(pPortData->tPortDef), OMX_PARAM_PORTDEFINITIONTYPE);
    if(OMX_GetParameter(pCompHandle, OMX_IndexParamPortDefinition, &(pPortData->tPortDef)) != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX_GetParameter failed\n");
    }

    MMS_IL_PRINT("Returning port index: %d pPortData 0x%08X on comp %d\n", (int)pPortData->tPortDef.nPortIndex, (unsigned int)pPortData, nComponentId);
    sem_post(&get_port_sem);
    return pPortData;
}

OMX_ERRORTYPE COMXComponent::GetOMXPortDef(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef, int nPort)
{
    sem_wait(&get_port_sem);
    OMX_INIT_STRUCT_PTR(pPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
    pPortDef->nPortIndex = nPort;
    OMX_ERRORTYPE ret  = OMX_GetParameter(pCompHandle, OMX_IndexParamPortDefinition, pPortDef);
    sem_post(&get_port_sem);
    return ret;
}

OMX_ERRORTYPE COMXComponent::AllocatePortBuffers(TIMM_OSAL_U32 nPort)
{
    TIMM_OSAL_U32 idx;
    TIMM_OSAL_U32 bufIdx;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
    OMX_CONFIG_RECTTYPE tFrameDim;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXPortHandle* pCompPorts = NULL;
    TIMM_OSAL_U32 nFrmWidth = 0, nFrmHeight = 0, nStride = 0;
    OMX_COLOR_FORMATTYPE nClrFmt = (OMX_COLOR_FORMATTYPE)0;

    MMS_IL_PRINT("Allocating buffers on OMX Component %d port %u\n", nComponentId, nPort);
    //Init frame dim structure
    OMX_INIT_STRUCT_PTR(&tFrameDim, OMX_CONFIG_RECTTYPE);
    tFrameDim.nPortIndex = nPort;
    //Get port definitions
    eError = GetOMXPortDef(&tPortDef, nPort);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX get port definitions failed\n");
        return eError;
    }
    //Get port data
    pCompPorts = GetPortData(tPortDef.nPortIndex);

    if(pCompPorts == NULL)
    {
        MMS_IL_PRINT("Unknown OMX port %d\n", (int)tPortDef.nPortIndex);
        return OMX_ErrorBadParameter;
    }

    //Get parameters for different domains
    //OMX_TI_IndexParam2DBufferAllocDimension gets frame dimenions
    switch (tPortDef.eDomain)
    {
        case OMX_PortDomainVideo:
            if (OMX_GetParameter(pCompHandle, (OMX_INDEXTYPE)OMX_TI_IndexParam2DBufferAllocDimension, &tFrameDim) == OMX_ErrorNone)
            {
                MMS_IL_PRINT("Acquired OMX_TI_IndexParam2DBufferAllocDimension data\n");
                nFrmWidth = tFrameDim.nWidth;
                nFrmHeight = tFrameDim.nHeight;

            }
            else
            {
                MMS_IL_PRINT("No OMX_TI_IndexParam2DBufferAllocDimension data\n");
                nFrmWidth = tPortDef.format.video.nFrameWidth;
                nFrmHeight = tPortDef.format.video.nFrameHeight;
            }
            nStride = tPortDef.format.video.nStride;
            nClrFmt = tPortDef.format.video.eColorFormat;
            break;
        case OMX_PortDomainImage:
            if (OMX_GetParameter(pCompHandle, (OMX_INDEXTYPE)OMX_TI_IndexParam2DBufferAllocDimension, &tFrameDim) == OMX_ErrorNone)
            {
                MMS_IL_PRINT("Acquired OMX_TI_IndexParam2DBufferAllocDimension data\n");
                nFrmWidth = tFrameDim.nWidth;
                nFrmHeight = tFrameDim.nHeight;
            }
            else
            {
                MMS_IL_PRINT("No OMX_TI_IndexParam2DBufferAllocDimension data\n");
                nFrmWidth = tPortDef.format.image.nFrameWidth;
                nFrmHeight = tPortDef.format.image.nFrameHeight;
            }
            nStride = tPortDef.format.image.nStride;
            nClrFmt = tPortDef.format.image.eColorFormat;
            break;
        default:
            MMS_IL_PRINT("!!!ERROR!!! Unknown Port Domain:%d\n", tPortDef.eDomain);
    }
    //For all ports for the given component

    for (idx = 0; (pCompPorts[idx].nPortNo != PORT_FINAL) && pCompPorts[idx].nPortNo != nPort; idx++);

    if (pCompPorts[idx].nPortNo == PORT_FINAL)
    {
        MMS_IL_PRINT("Failed to identify port index %d in OMX component %d ports\n", nPort, nComponentId);
        return OMX_ErrorBadParameter;
    }

    if (pCompPorts[idx].pComponentBuffers)
    {
        MMS_IL_PRINT("Port buffers already allocated - leaving without allocating new ones\n");
        return OMX_ErrorNone;
    }

    if (pCompPorts[idx].bSupplierPort == TIMM_OSAL_FALSE)
    {
        MMS_IL_PRINT("Consumer port - attempting to establish component-port link\n");
        return LinkPortBuffers(nPort);
    }

    // Allocate Space for Buffer Pointers Array
    pCompPorts[idx].pComponentBuffers = (COMXCompBufferDesc*)TIMM_OSAL_MallocExtn(sizeof(COMXCompBufferDesc) * tPortDef.nBufferCountActual, TIMM_OSAL_TRUE, 32, TIMMOSAL_MEM_SEGMENT_EXT, NULL);

    if (!pCompPorts[idx].pComponentBuffers)
    {
        MMS_IL_PRINT("No memory resource for buffer header structure\n");
        return OMX_ErrorInsufficientResources;
    }

    pCompPorts[idx].nBufferCount = tPortDef.nBufferCountActual;

    // Set linked component buffers and actual buffer pointer to NULL - there is no linked component yet
    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        pCompPorts[idx].pComponentBuffers[bufIdx].pCorespBuffer = NULL;
        pCompPorts[idx].pComponentBuffers[bufIdx].pActualPtr = NULL;
        pCompPorts[idx].pComponentBuffers[bufIdx].bFree = TIMM_OSAL_TRUE;
    }

    // Gralloc is not supported yet - set DOMX to use vtprs
    OMX_TI_PARAMUSENATIVEBUFFER useGrallocHandles;
    OMX_INIT_STRUCT_PTR(&useGrallocHandles, OMX_TI_PARAMUSENATIVEBUFFER);

    useGrallocHandles.nPortIndex = nPort;
    useGrallocHandles.bEnable = OMX_FALSE;
    //Disable gralloc handles
    eError = OMX_SetParameter(pCompHandle, (OMX_INDEXTYPE)OMX_TI_IndexUseNativeBuffers, &useGrallocHandles);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX set OMX_TI_IndexUseNativeBuffers failed\n");
        return eError;
    }

    // Disable extended buffer by default
    OMX_TI_PARAM_USEBUFFERDESCRIPTOR bufDescriptor;
    OMX_INIT_STRUCT_PTR(&bufDescriptor, OMX_TI_PARAM_USEBUFFERDESCRIPTOR);
    bufDescriptor.nPortIndex = nPort;
    bufDescriptor.bEnabled = OMX_FALSE;
    bufDescriptor.eBufferType = OMX_TI_BufferTypeVirtual2D;

    eError = OMX_SetParameter(pCompHandle, (OMX_INDEXTYPE)OMX_TI_IndexUseBufferDescriptor, &bufDescriptor);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX set OMX_TI_IndexUseBufferDescriptor failed. Error - 0x%08X\n", eError);
       // return eError;
    }

    if (0 == tPortDef.nBufferCountActual) {
        MMS_IL_PRINT("\n\n!!! ERROR: nBufferCountActual = 0 for Component = %d on port %d\n\n", nComponentId, (int)nPort);
    }

    // Allocate Buffers
    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        OMX_BUFFERHEADERTYPE **ppBuffer = &(pCompPorts[idx].pComponentBuffers[bufIdx].pBuffer);
        //int Bpp = 0;
        if (pCompPorts[idx].eAllocType == MEM_ION_1D)
        {
            MMS_IL_PRINT("Calling MEM_ION_1D on port %d of comp %d\n", nPort, nComponentId);
            CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
            OMX_U32 nSize = ALIGN(tPortDef.nBufferSize, 4096);
            //OMX_U32 nSize = ALIGN(nFrmWidth*nFrmHeight*Bpp, 4096);
            //allocate 1D buffer
            TIMM_OSAL_PTR pBuf = ionAlloc->alloc(nSize, 4096, nStride);
            if (pBuf)
            {
                if (pCompPorts[idx].nDisplayId != -1 && displayModule != NULL && (tPortDef.eDomain == OMX_PortDomainVideo || tPortDef.eDomain == OMX_PortDomainImage))
                {
                    //in case of video domain
                    PerFrameConfiguration cfg;
                    cfg.format = nClrFmt;
                    cfg.width  = nFrmWidth;
                    cfg.height = nFrmHeight;
                    cfg.stride = nFrmWidth;
                    //Config display overlay
                    if (displayModule->PerFrameConfig(cfg) != 0)
                    {
                        ionAlloc->free(pBuf);
                        return OMX_ErrorUndefined;
                    }
                }
                MMS_IL_PRINT("Setting ION allocated buffer %p of comp %d (length %lu) to OMX component port %u\n", pBuf, nComponentId, nSize, nPort);
                eError = OMX_UseBuffer(pCompHandle, ppBuffer, nPort, NULL, nSize, (OMX_U8*)pBuf);
                MMS_IL_PRINT("OMX_UseBuffer returned - 0x%08X\n", eError);

                pCompPorts[idx].pComponentBuffers[bufIdx].pActualPtr = pBuf;
            }
            else
            {
                MMS_IL_PRINT("Frame buffer allocation failed\n");
                return OMX_ErrorInsufficientResources;
            }
        }
        else if (pCompPorts[idx].eAllocType == MEM_ION_2D)
        {
            MMS_IL_PRINT("Calling MEM_ION_2D on port %d of comp %d\n", nPort, nComponentId);
            int fmt;
            unsigned int stride;
            OMX_TI_BUFFERDESCRIPTOR_TYPE *Ion2DBuf;
            CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
            //Set color format
            switch(nClrFmt)
            {
                case OMX_COLOR_FormatYUV420SemiPlanar:
                case OMX_COLOR_FormatYUV420PackedSemiPlanar:
                    fmt = TILER_PIXEL_FMT_8BIT;
                    break;
                case OMX_COLOR_FormatCbYCrY:
                case OMX_COLOR_Format16bitRGB565:
                    fmt = TILER_PIXEL_FMT_16BIT;
                    break;
                case OMX_COLOR_Format32bitARGB8888:
                    fmt = TILER_PIXEL_FMT_32BIT;
                    break;
                default:
                    MMS_IL_PRINT("Unsupported color format (%u)\n", nClrFmt);
                    return OMX_ErrorBadParameter;
            }

            //Enable extended buffer
            bufDescriptor.bEnabled = OMX_TRUE;

            eError = OMX_SetParameter(pCompHandle, (OMX_INDEXTYPE)OMX_TI_IndexUseBufferDescriptor, &bufDescriptor);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("OMX set OMX_TI_IndexUseBufferDescriptor failed\n");
                return eError;
            }

            Ion2DBuf = (OMX_TI_BUFFERDESCRIPTOR_TYPE *)malloc(sizeof(OMX_TI_BUFFERDESCRIPTOR_TYPE));
            memset((void *) Ion2DBuf, 0, sizeof(OMX_TI_BUFFERDESCRIPTOR_TYPE));
            Ion2DBuf->nSize = sizeof(OMX_TI_BUFFERDESCRIPTOR_TYPE);
            Ion2DBuf->eBufType = OMX_TI_BufferTypeVirtual2D;
            Ion2DBuf->nNumOfBuf = 1;
            //Allocated 2d buffer and add it to ionHdls vector(for Luma)
            Ion2DBuf->pBuf[0] = ionAlloc->alloc2D(nFrmWidth, nFrmHeight, fmt, &stride);
            if (Ion2DBuf->pBuf[0])
            {
                if (pCompPorts[idx].nDisplayId != -1 && displayModule != NULL && (tPortDef.eDomain == OMX_PortDomainVideo || tPortDef.eDomain == OMX_PortDomainImage))
                {
                    PerFrameConfiguration cfg;
                    cfg.format = nClrFmt;
                    cfg.width = nFrmWidth;
                    cfg.height = nFrmHeight;
                    cfg.stride = stride;

                    if (displayModule->PerFrameConfig(cfg) != 0)
                    {
                        ionAlloc->free(Ion2DBuf->pBuf[0]);
                        free(Ion2DBuf);
                        return OMX_ErrorUndefined;
                    }
                }

                if (nClrFmt == OMX_COLOR_FormatYUV420SemiPlanar || nClrFmt == OMX_COLOR_FormatYUV420PackedSemiPlanar)
                {
                    Ion2DBuf->nNumOfBuf = 2;
                    //Allocated 2d buffer and add it to ionHdls vector for Chroma
                    Ion2DBuf->pBuf[1] = ionAlloc->alloc2D(nFrmWidth / 2, nFrmHeight / 2, TILER_PIXEL_FMT_16BIT , &stride);
                    if (Ion2DBuf->pBuf[1] == NULL)
                    {
                        ionAlloc->free(Ion2DBuf->pBuf[0]);
                        free(Ion2DBuf);
                        MMS_IL_PRINT("UV Frame buffer allocation failed\n");
                        return OMX_ErrorInsufficientResources;
                    }
                }

                MMS_IL_PRINT("Setting ION 2D allocated buffer %p of comp %d, size %lu (%u x %u) to OMX component port %u\n", Ion2DBuf->pBuf[0], nComponentId, tPortDef.nBufferSize, nFrmWidth, nFrmHeight, nPort);
                eError = OMX_UseBuffer(pCompHandle, ppBuffer, nPort, NULL, tPortDef.nBufferSize, (OMX_U8 *) Ion2DBuf);
                MMS_IL_PRINT("OMX_UseBuffer returned - 0x%08X\n", eError);

                pCompPorts[idx].pComponentBuffers[bufIdx].pActualPtr = (TIMM_OSAL_PTR) Ion2DBuf;
            }
            else
            {
                MMS_IL_PRINT("Frame buffer allocation failed\n");
                return OMX_ErrorInsufficientResources;
            }
        }
        else if (pCompPorts[idx].eAllocType == MEM_OSAL)
        {//Allocate buffers usong OSAL metods
            TIMM_OSAL_PTR pBuf = TIMM_OSAL_MallocExtn(tPortDef.nBufferSize, TIMM_OSAL_TRUE, tPortDef.nBufferAlignment, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
            if (pBuf)
            {
                MMS_IL_PRINT("Setting TIMM_OSAL allocated buffer %p to OMX component %d port %u\n", pBuf, nComponentId, nPort);
                eError = OMX_UseBuffer(pCompHandle, ppBuffer, nPort, NULL, tPortDef.nBufferSize, (OMX_U8*)pBuf);
                pCompPorts[idx].pComponentBuffers[bufIdx].pActualPtr = pBuf;
            }
            else
            {
                MMS_IL_PRINT("TIMM_OSAL buffer allocation failed\n");
                return OMX_ErrorInsufficientResources;
            }
        }
        else if (pCompPorts[idx].eAllocType == MEM_OMX)
        {//Allocate buffers using OMX_AllocateBuffer macro
            MMS_IL_PRINT("Calling OMX_AllocBuffer on OMX component %p port %u\n", pCompHandle, nPort);
            eError = OMX_AllocateBuffer(pCompHandle, ppBuffer, nPort, NULL, tPortDef.nBufferSize);
        }
        else
        {
            MMS_IL_PRINT("Unrecognized buffer allocation method %d\n", pCompPorts[idx].eAllocType);
            return OMX_ErrorBadParameter;
        }

        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("OMX Component %p returned error 0x%x on using port %u buffer %u\n", pCompHandle, eError, nPort, bufIdx);
            return eError;
        }
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::FillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    return OMX_FillThisBuffer(pCompHandle, pBuffer);
}

OMX_ERRORTYPE COMXComponent::QueueFillPortBuffers(OMX_U32 nPort, OMX_U32 bufIdx)
{
    COMXPortHandle *pCompPort;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    MMS_IL_PRINT("Queue fill buffer on OMX Component %p with ID %d, port %lu\n", pCompHandle, nComponentId, nPort);

    pCompPort = GetPortData(nPort);
    if (pCompPort == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    if (OMX_DirOutput != pCompPort->tPortDef.eDir)
    {
        MMS_IL_PRINT("Exiting - specified port is not an output port\n");
        return OMX_ErrorNone;
    }

   if (pCompPort->nBufferCount == 0)
   {
        MMS_IL_PRINT("Exiting - no buffers allocated on port\n");
        return OMX_ErrorNone;
   }
   if (bufIdx == 0xFF)
   {
        //Fill all buffers
        for (unsigned int buffIdx = 0; buffIdx < pCompPort->tPortDef.nBufferCountActual; buffIdx++)
        {
            if (pCompPort->pComponentBuffers[buffIdx].bFree == TIMM_OSAL_TRUE)
            {
                MMS_IL_PRINT("Calling OMX_FillThisBuffer on buffer %p, buffIdx - %d\n", pCompPort->pComponentBuffers[buffIdx].pBuffer, buffIdx);
                eError = OMX_FillThisBuffer(pCompHandle, pCompPort->pComponentBuffers[buffIdx].pBuffer);
                if (eError != OMX_ErrorNone)
                {
                    MMS_IL_PRINT("OMX_FillThisBuffer failed\n");
                    return eError;
                }
                pCompPort->pComponentBuffers[buffIdx].bFree = TIMM_OSAL_FALSE;
            } else
            {
                MMS_IL_PRINT("Buffer %p is not free, buffIdx - %d\n", pCompPort->pComponentBuffers[buffIdx].pBuffer, buffIdx);
            }//if (pCompPort->pComponentBuffers[buffIdx].bFree == TIMM_OSAL_TRUE)
        }// for (int buffIdx = 0; buffIdx < tPortDef.nBufferCountActual; buffIdx++)
   } else //fill only selected one
   {
        if (pCompPort->pComponentBuffers[bufIdx].bFree == TIMM_OSAL_TRUE)
        {
            MMS_IL_PRINT("Calling OMX_FillThisBuffer on buffer %p\n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
            eError = OMX_FillThisBuffer(pCompHandle, pCompPort->pComponentBuffers[bufIdx].pBuffer);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("OMX_FillThisBuffer failed\n");
                return eError;
            }
            pCompPort->pComponentBuffers[bufIdx].bFree = TIMM_OSAL_FALSE;
        }
        else
        {
            MMS_IL_PRINT("Buffer %p is not free\n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
        } //if (pCompPort->pComponentBuffers[bufIdx].bFree == TIMM_OSAL_TRUE)
   }//if (bufIdx == 0xFF)

    return eError;
}

OMX_ERRORTYPE COMXComponent::EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
    return OMX_EmptyThisBuffer(pCompHandle, pBuffer);
}

OMX_ERRORTYPE COMXComponent::QueueEmptyPortBuffers(OMX_U32 nPort, OMX_U32 bufIdx, bool bSend2linkedComp)
{
    COMXPortHandle *pCompPort;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    void* pCompHndlTemp = NULL;
    MMS_IL_PRINT("Queue empty buffer on OMX Component %p with ID %d, port %lu\n", pCompHandle, nComponentId, nPort);
    //Get Port definitions, call OMX_GetParameter

    //Gets the posrt data from port handle vector
    pCompPort = GetPortData(nPort);
    if (pCompPort == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    if (OMX_DirInput != pCompPort->tPortDef.eDir)
    {
        MMS_IL_PRINT("Exiting - specified port is not an input port\n");
        return OMX_ErrorNone;
    }
    if (bSend2linkedComp)
    {
        pCompHndlTemp = pCompPort->pLinkedComponent;
        MMS_IL_PRINT("Sending buffer to linked component!\n");
    } else pCompHndlTemp = pCompHandle;

    if (pCompPort->nBufferCount == 0)
    {
        MMS_IL_PRINT("Exiting - no buffers allocated on port\n");
        return OMX_ErrorNone;
    }

    if (bufIdx == 0xFF)
    {
        //Free all buffers allocated for this port
        for (bufIdx = 0; bufIdx < pCompPort->tPortDef.nBufferCountActual; bufIdx++)
        {
            if (pCompPort->pComponentBuffers[bufIdx].bFree == TIMM_OSAL_TRUE)
            {
                MMS_IL_PRINT("Calling OMX_EmptyThisBuffer on buffer %p\n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
                eError = OMX_EmptyThisBuffer(pCompHndlTemp, pCompPort->pComponentBuffers[bufIdx].pBuffer);
                if (eError != OMX_ErrorNone)
                {
                    MMS_IL_PRINT("OMX_EmptyThisBuffer failed\n");
                    return eError;
                }
                pCompPort->pComponentBuffers[bufIdx].bFree = TIMM_OSAL_FALSE;
            }
            else
            {
                MMS_IL_PRINT("Buffer %p is not free\n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
            }
        }
    } else
    {
         if (pCompPort->pComponentBuffers[bufIdx].bFree == TIMM_OSAL_TRUE)
            {
                MMS_IL_PRINT("Calling OMX_EmptyThisBuffer on buffer %p of \n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
                eError = OMX_EmptyThisBuffer(pCompHandle, pCompPort->pComponentBuffers[bufIdx].pBuffer);
                if (eError != OMX_ErrorNone)
                {
                    MMS_IL_PRINT("OMX_EmptyThisBuffer failed\n");
                    return eError;
                }
                pCompPort->pComponentBuffers[bufIdx].bFree = TIMM_OSAL_FALSE;
            }
            else
            {
                MMS_IL_PRINT("Buffer %p is not free\n", pCompPort->pComponentBuffers[bufIdx].pBuffer);
            }
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::FreeOMXBuffer(OMX_BUFFERHEADERTYPE* pBuffer, int nPort)
{
    return OMX_FreeBuffer(pCompHandle, nPort, pBuffer);
}

OMX_ERRORTYPE COMXComponent::FreePortBuffers(TIMM_OSAL_U32 nPort)
{
    TIMM_OSAL_U32 bufIdx;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXPortHandle *pCompPort;

    MMS_IL_PRINT("Freeing buffers on OMX Component %p with id %d port %u\n", pCompHandle, nComponentId, nPort);

    eError = GetOMXPortDef(&tPortDef, nPort);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX get port definitions failed\n");
        return eError;
    }

    pCompPort = GetPortData(nPort);
    if (pCompPort == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    if (!pCompPort->pComponentBuffers)
    {
        MMS_IL_PRINT("No port buffers allocated for port %d on comp %d- leaving without attempting to free\n", nPort, nComponentId);
        return OMX_ErrorNone;
    }

    if (pCompPort->nBufferCount == 0)
    {
        MMS_IL_PRINT("Buffer count of zero - leaving without attempting to free\n");
        return OMX_ErrorNone;
    }

    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        pCompPort->pComponentBuffers[bufIdx].pCorespBuffer = NULL;
    }

    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        if (pCompPort->pComponentBuffers[bufIdx].pBuffer)
        {

            if (tPortDef.bEnabled)
            {
                MMS_IL_PRINT("calling OMX_FreeBuffer on comp %d\n", nComponentId);
                eError = OMX_FreeBuffer(pCompHandle, nPort, pCompPort->pComponentBuffers[bufIdx].pBuffer);
            }

            if (pCompPort->eAllocType == MEM_ION_1D)
            {
                CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                if (pCompPort->pComponentBuffers[bufIdx].pActualPtr != NULL)
                {
                    MMS_IL_PRINT("calling ION_Free on %p of comp %d\n", pCompPort->pComponentBuffers[bufIdx].pActualPtr, nComponentId);
                    ionAlloc->free(pCompPort->pComponentBuffers[bufIdx].pActualPtr);
                }
            }
            else if (pCompPort->eAllocType == MEM_ION_2D)
            {
                CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                OMX_TI_BUFFERDESCRIPTOR_TYPE *Ion2DBuf = (OMX_TI_BUFFERDESCRIPTOR_TYPE *)(pCompPort->pComponentBuffers[bufIdx].pActualPtr);
                if (Ion2DBuf)
                {
                    for (unsigned int i = 0; i < Ion2DBuf->nNumOfBuf; i++)
                    {
                        if (Ion2DBuf->pBuf[i])
                        {
                            MMS_IL_PRINT("calling ION_Free on %p of comp %d\n", Ion2DBuf->pBuf[i], nComponentId);
                            ionAlloc->free(Ion2DBuf->pBuf[i]);
                        }
                    }
                    free(Ion2DBuf);
                }
            }
            else if (pCompPort->eAllocType == MEM_OSAL)
            {
                if (pCompPort->pComponentBuffers[bufIdx].pActualPtr != NULL)
                {
                    MMS_IL_PRINT("calling TIMM_OSAL_Free on %p of comp %d\n", pCompPort->pComponentBuffers[bufIdx].pActualPtr, nComponentId);
                    TIMM_OSAL_Free(pCompPort->pComponentBuffers[bufIdx].pActualPtr);
                }
                else
                {
                    MMS_IL_PRINT("Cannot find mapped pointer\n");
                }
            }

            pCompPort->pComponentBuffers[bufIdx].pBuffer = NULL;
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("OMX Component returned error on OMX_FreeBuffer of buffer idnex %u\n", bufIdx);
                return eError;
            }
        }
    }

    TIMM_OSAL_Free(pCompPort->pComponentBuffers);
    pCompPort->pComponentBuffers = NULL;

    return eError;
}

OMX_ERRORTYPE COMXComponent::FreeAllPortBuffers()
{
    TIMM_OSAL_U32 idx , end;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    OMX_INIT_STRUCT_PTR(&tPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

    MMS_IL_PRINT("Freeing all buffers on OMX Component %p with id %d\n", pCompHandle, nComponentId);

  // MMS_IL_PRINT("VideoPortStartIndex() - %d, VideoPortCount() - %d \n",VideoPortStartIndex(), VideoPortCount());
   //MMS_IL_PRINT("AudioPortStartIndex() - %d, AudioPortCount() - %d \n",AudioPortStartIndex(), AudioPortCount());
   //MMS_IL_PRINT("ImagePortStartIndex() - %d, ImagePortCount() - %d \n",ImagePortStartIndex(), ImagePortCount());
   //MMS_IL_PRINT("OtherPortStartIndex() - %d, OtherPortCount() - %d \n",OtherPortStartIndex(), OtherPortCount());

    idx = VideoPortStartIndex();
    end = idx + VideoPortCount();
    for(unsigned int i = idx; i < end; i++)
    {
        eError = FreePortBuffers(i);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("ERROR 0x%08x\n", eError);
        }
    }

    idx = AudioPortStartIndex();
    end = idx + AudioPortCount();
    for(unsigned int i = idx; i < end; i++)
    {
        eError = FreePortBuffers(i);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("ERROR 0x%08x\n", eError);
        }
    }

    idx = ImagePortStartIndex();
    end = idx + ImagePortCount();
    for(unsigned int i = idx; i < end; i++)
    {
        eError = FreePortBuffers(i);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("ERROR 0x%08x\n", eError);
        }
    }

    idx = OtherPortStartIndex();
    end = idx + OtherPortCount();
    for(unsigned int i = idx; i < end; i++)
    {
        eError = FreePortBuffers(i);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("ERROR 0x%08x\n", eError);
        }
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::LinkPortBuffers(TIMM_OSAL_U32 nPort)
{
    TIMM_OSAL_U32 bufIdx;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXPortHandle *pCompPort;

    MMS_IL_PRINT("Linking ports on OMX Component %p\n", pCompHandle);

    pCompPort = GetPortData(nPort);
    if (pCompPort == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    if (!pCompPort->pLinkedPort)
    {
        MMS_IL_PRINT("Exiting - no port linking information present\n");
        return OMX_ErrorBadParameter;
    }

    if (pCompPort->pComponentBuffers)
    {
        MMS_IL_PRINT("Exiting - port buffers already allocated\n");
        return OMX_ErrorBadParameter;
    }

    pCompPort->pComponentBuffers = (COMXCompBufferDesc*)TIMM_OSAL_MallocExtn(sizeof(COMXCompBufferDesc) * pCompPort->tPortDef.nBufferCountActual, TIMM_OSAL_TRUE, 32, TIMMOSAL_MEM_SEGMENT_EXT, NULL);

    pCompPort->nBufferCount = pCompPort->tPortDef.nBufferCountActual;
    MMS_IL_PRINT("Linking to Component %p with id %d on port %u\n", ((COMXComponent*)(pCompPort->pLinkedComponent))->pCompHandle, nComponentId, pCompPort->pLinkedPort->nPortNo);

    for (bufIdx = 0; bufIdx < pCompPort->tPortDef.nBufferCountActual; bufIdx++)
    {
        if (pCompPort->pLinkedPort && pCompPort->pLinkedPort->pComponentBuffers[bufIdx].pBuffer)
        {
            pCompPort->pComponentBuffers[bufIdx].pCorespBuffer = pCompPort->pLinkedPort->pComponentBuffers[bufIdx].pBuffer;
            pCompPort->pComponentBuffers[bufIdx].pActualPtr = pCompPort->pLinkedPort->pComponentBuffers[bufIdx].pActualPtr;
            MMS_IL_PRINT("Calling OMX_UseBuffer for buffer %p on port %u for component %d\n", pCompPort->pComponentBuffers[bufIdx].pCorespBuffer->pBuffer, nPort, nComponentId);

            eError = OMX_UseBuffer(pCompHandle, &(pCompPort->pComponentBuffers[bufIdx].pBuffer), nPort, NULL, pCompPort->tPortDef.nBufferSize, pCompPort->pComponentBuffers[bufIdx].pCorespBuffer->pBuffer);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("Exiting - OMX_UseBuffer call failed\n");
                return eError;
            }
            pCompPort->pLinkedPort->pComponentBuffers[bufIdx].pCorespBuffer = pCompPort->pComponentBuffers[bufIdx].pBuffer;
        }
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::SetStateExecuting()
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    if (ComponentState == OMX_StateExecuting)
    {
        MMS_IL_PRINT("Component %d already in EXECUTING state\n", nComponentId);
        return OMX_ErrorNone;
    }

    MMS_IL_PRINT("Attempting to placing OMX component %p with ID %d in EXECUTING state\n", pCompHandle, nComponentId);

    switch (ComponentState)
    {
        case OMX_StateLoaded:
            if (LastStateCommand == OMX_StateIdle)
            {
                MMS_IL_PRINT("Current %d ComponentState: LOADED->IDLE\n", nComponentId);
                eError = WaitForState(OMX_StateIdle);
                if (eError != OMX_ErrorNone)
                {
                    //MMS_IL_PRINT("WaitForState returned error 0x%08X\n", eError);
                    return eError;
                }
            } else
            {
                MMS_IL_PRINT("Current %d ComponentState: LOADED\n", nComponentId);
                return OMX_ErrorNone;
            }
            // intentional fall-through
        case OMX_StateIdle:
            MMS_IL_PRINT("Current %d ComponentState: IDLE\n", nComponentId);
            if (TIMM_OSAL_ERR_NONE != buffMgr->Start())
            {
                MMS_IL_PRINT("COMXBuffDoneManager Start() failed\n");
                return OMX_ErrorUndefined;
            }

            eError = OMX_SendCommand(pCompHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("OMX_SendCommand returned error 0x%08X\n", eError);
                return eError;
            }

            eError = WaitForState(OMX_StateExecuting);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("WaitForState returned error 0x%08X\n", eError);
                buffMgr->Stop();
                return eError;
            }
            break;
        default:
            MMS_IL_PRINT("Wrong ComponentState: %d\n", ComponentState);
            eError = OMX_ErrorUndefined;
            break;
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::SetStateLoaded()
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    if (ComponentState == OMX_StateLoaded)
    {
        MMS_IL_PRINT("Component %d already in LOADED state\n", nComponentId);
        return OMX_ErrorNone;
    }

    MMS_IL_PRINT("Attempting to placing OMX component %p with ID %d in LOADED state\n", pCompHandle, nComponentId);
    //Component current state
    switch (ComponentState)
    {
        case OMX_StateExecuting:
            if (LastStateCommand == OMX_StateIdle)
            {//Direct transition from State Executing to State Loaded is not possible
                //Go to State Idle first
                MMS_IL_PRINT("Current %d ComponentState: EXECUTING->IDLE\n", nComponentId);
                //Hang untill state is changed
                eError = WaitForState(OMX_StateIdle);
                if (eError != OMX_ErrorNone)
                {
                    //MMS_IL_PRINT("WaitForState returned error 0x%08X\n", eError);
                    return eError;
                }
            }else
            {
                MMS_IL_PRINT("Current %d ComponentState: EXECUTING\n", nComponentId);
                return OMX_ErrorNone;
            }
            // intentional fall-through
        case OMX_StateIdle:
            MMS_IL_PRINT("Current %d ComponentState: IDLE\n", nComponentId);
            //Go to state Loaded
            eError = OMX_SendCommand(pCompHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("OMX_SendCommand returned error 0x%08X\n", eError);
                return eError;
            }
            //In order for setting state Loaded
            //All buffers need to be freed
            eError = FreeAllPortBuffers();
            if (eError != OMX_ErrorNone)
            {
                MMS_IL_PRINT("FreeAllPortBuffers returned error 0x%08X\n", eError);
                return eError;
            }
            eError = WaitForState(OMX_StateLoaded);
            if (eError != OMX_ErrorNone)
            {
                //MMS_IL_PRINT("WaitForState returned error 0x%08X\n", eError);
                return eError;
            }
            break;
        default:
            MMS_IL_PRINT("Wrong ComponentState: %d\n", ComponentState);
            eError = OMX_ErrorUndefined;
            break;
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::SetStateIdle()
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    if (ComponentState == OMX_StateIdle)
    {
        MMS_IL_PRINT("Component %d already in IDLE state\n", nComponentId);
        return OMX_ErrorNone;
    }

    MMS_IL_PRINT("Placing OMX component %p with id %d in IDLE state\n", pCompHandle, nComponentId);

    switch (ComponentState)
    {
        case OMX_StateLoaded:
            MMS_IL_PRINT("Current %d ComponentState: LOADED\n", nComponentId);
            //Component will got to state Wait for Resources first
            //and wait for all buffers to be allocated and passed
            eError = OMX_SendCommand(pCompHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
            if (eError != OMX_ErrorNone)
            {
                return eError;
            }
            break;
        case OMX_StateExecuting:
            MMS_IL_PRINT("Current %d ComponentState: EXECUTING\n", nComponentId);
            //return all buffers to their respective suppliers
            buffMgr->Stop();
            // MMS_IL_PRINT("buffMgr is stopped!\n");
            if(displayModule != NULL)
            {
                // Passing NULL as buffer addr will disable DSS overlays
                displayModule->DisplayFrame(NULL, NULL, 0, 0, 0, 0);
            }
             //MMS_IL_PRINT("Display is disabled! Stopping execution!\n");

            //Execute command Set state Idle
            eError = OMX_SendCommand(pCompHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
            if (eError != OMX_ErrorNone)
            {
                return eError;
            }
            eError = WaitForState(OMX_StateIdle);
            if (eError != OMX_ErrorNone)
            {
                return eError;
            }
            break;
        default:
            eError = OMX_ErrorUndefined;
            break;
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::WaitForState(OMX_STATETYPE DesiredState)
{
    OMX_STATETYPE CurState = OMX_StateInvalid;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
//    TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
//    TIMM_OSAL_U32 pRetrievedEvents;

    eError = OMX_GetState(pCompHandle, &CurState);
    if(eError != OMX_ErrorNone)
    {
        return eError;
    }

    MMS_IL_PRINT("Waiting for OMX Component %p to enter state: %u\n", pCompHandle, DesiredState);

    while (CurState != DesiredState)
    {  //Wait for Event Handler callback to release sempahore
        //retval = TIMM_OSAL_EventRetrieve(pComponentEvents, COMPONENT_EVENT_CMD_STATE_CHANGE,
                             //       TIMM_OSAL_EVENT_AND_CONSUME, &pRetrievedEvents, 300000);

      //  if(retval != TIMM_OSAL_ERR_NONE)
       // {
         //   MMS_IL_PRINT("TIMM_OSAL_EventRetrieve failed with : 0x%08X\n", retval);
          //  return OMX_ErrorUndefined;
       // }
        sem_wait(&state_set_sem);
        //Get current state
        eError = OMX_GetState(pCompHandle, &CurState);
        if(eError != OMX_ErrorNone)
        {
            return eError;
        }
        MMS_IL_PRINT("OMX Component reported state: %u\n", CurState);
    }

    ComponentState = CurState;

    return eError;
}

OMX_ERRORTYPE COMXComponent::WaitForPortDisable()
{
    TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_U32 pRetrievedEvents;
    //Wait for Event Handler callback to release sempahore
    retval = TIMM_OSAL_EventRetrieve(pComponentEvents, COMPONENT_EVENT_CMD_PORT_DISABLE, TIMM_OSAL_EVENT_AND_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);

    if(retval == TIMM_OSAL_WAR_TIMEOUT || retval != TIMM_OSAL_ERR_NONE)
    {
        return OMX_ErrorTimeout;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXComponent::WaitForPortEnable()
{
    TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_U32 pRetrievedEvents;
    //Wait for Event Handler callback to release sempahore
    retval = TIMM_OSAL_EventRetrieve(pComponentEvents, COMPONENT_EVENT_CMD_PORT_ENABLE, TIMM_OSAL_EVENT_AND_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);

    if(retval == TIMM_OSAL_WAR_TIMEOUT || retval != TIMM_OSAL_ERR_NONE)
    {
        return OMX_ErrorTimeout;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE InputPortCallback(COMXPortHandle* pPortLink, OMX_BUFFERHEADERTYPE *pBuffer)
{
    TIMM_OSAL_U32 nIndex;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXComponent* pComp = (COMXComponent*)pPortLink->pLinkedComponent;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

    MMS_IL_PRINT("Input port on OMX Component %p done\n", pComp->OMXCompHdl());

    for (nIndex = 0; nIndex < pPortLink->nBufferCount; nIndex++)
    {
        MMS_IL_PRINT("Comparing %p to %p\n", pBuffer, pPortLink->pComponentBuffers[nIndex].pBuffer);
        if (pBuffer == pPortLink->pComponentBuffers[nIndex].pBuffer)
        {
            break;
        }
    }

    if (pComp->OMXCompHdl() && (nIndex < pPortLink->nBufferCount))
    {
        eError = pComp->GetOMXPortDef(&tPortDef,  pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nOutputPortIndex);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("commonInputPortCallback - error getting port %lu parameters\n", tPortDef.nPortIndex);
            return eError;
        }

        if (tPortDef.bEnabled == OMX_FALSE)
        {
            MMS_IL_PRINT("commonInputPortCallback - %lu port is disabled - freeing buffer\n", tPortDef.nPortIndex);
            OMX_FreeBuffer(pComp->OMXCompHdl(), tPortDef.nPortIndex, pPortLink->pComponentBuffers[nIndex].pCorespBuffer);
        }
        else if (pPortLink->bAutoBufferCycle)
        {
            MMS_IL_PRINT("commonInputPortCallback - Calling OMX_FillThisBuffer on %lu port\n", tPortDef.nPortIndex);
            eError = OMX_FillThisBuffer(pComp->OMXCompHdl(), pPortLink->pComponentBuffers[nIndex].pCorespBuffer);
        }
        else
        {
            pPortLink->pComponentBuffers[nIndex].bFree = TIMM_OSAL_TRUE;
        }
    }
    else
    {
        MMS_IL_PRINT("commonInputPortCallback failed to identify buffer on linked component\n");
    }

    return eError;
}

OMX_ERRORTYPE OutputPortCallback(COMXPortHandle *pPortLink, OMX_BUFFERHEADERTYPE *pBuffer)
{
    TIMM_OSAL_U32 nIndex;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXComponent* pComp = (COMXComponent*)pPortLink->pLinkedComponent;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

    MMS_IL_PRINT("commonOutputPortCallback on OMX Component %p\n", pComp->OMXCompHdl());

    for (nIndex = 0; nIndex < pPortLink->nBufferCount; nIndex++)
    {
        MMS_IL_PRINT("Comparing %p to %p\n", pBuffer, pPortLink->pComponentBuffers[nIndex].pBuffer);
        if (pBuffer == pPortLink->pComponentBuffers[nIndex].pBuffer)
        {
            break;
        }
    }

    if (pComp->OMXCompHdl() && (nIndex < pPortLink->nBufferCount))
    {
        eError = pComp->GetOMXPortDef(&tPortDef,  pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nInputPortIndex);
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("commonOutputPortCallback - error getting port %lu parameters\n", tPortDef.nPortIndex);
            return eError;
        }

        if (tPortDef.bEnabled == OMX_FALSE)
        {
            MMS_IL_PRINT("commonOutputPortCallback - %lu port is disabled - freeing buffer\n", tPortDef.nPortIndex);
            OMX_FreeBuffer(pComp->OMXCompHdl(), tPortDef.nPortIndex, pPortLink->pComponentBuffers[nIndex].pCorespBuffer);
        }
        else if(pPortLink->bAutoBufferCycle)
        {
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nFilledLen = pBuffer->nFilledLen;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nOffset = pBuffer->nOffset;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->hMarkTargetComponent = pBuffer->hMarkTargetComponent;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->pMarkData = pBuffer->pMarkData;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nTickCount = pBuffer->nTickCount;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nTimeStamp = pBuffer->nTimeStamp;
            pPortLink->pComponentBuffers[nIndex].pCorespBuffer->nFlags = pBuffer->nFlags;

            MMS_IL_PRINT("commonOutputPortCallback - Calling OMX_EmptyThisBuffer on port%lu \n", tPortDef.nPortIndex);

            eError = OMX_EmptyThisBuffer(pComp->OMXCompHdl(), pPortLink->pComponentBuffers[nIndex].pCorespBuffer);
        }
        else
        {
            pPortLink->pComponentBuffers[nIndex].bFree = TIMM_OSAL_TRUE;
        }
    }
    else
    {
        MMS_IL_PRINT("commonOutputPortCallback failed to identify buffer on linked component\n");
    }

    return eError;
}

OMX_ERRORTYPE COMXComponent::UseMarshalledMemOnPortBuffers(unsigned int nPort, unsigned int nBuffCnt, unsigned char** ppBuffs)
{
    TIMM_OSAL_U32 bufIdx;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    COMXPortHandle *pCompPort;
    TIMM_OSAL_U32 i, ii;

    MMS_IL_PRINT("UseMarshalledMemOnComponentPortBuffers on OMX Component %p\n", pCompHandle);

    eError = GetOMXPortDef(&tPortDef, nPort);
    if (eError != OMX_ErrorNone)
    {
        MMS_IL_PRINT("OMX get port definitions failed\n");
        return eError;
    }

    pCompPort = GetPortData(nPort);
    if (pCompPort == NULL)
    {
        return OMX_ErrorUndefined;
    }

    if (pCompPort->pComponentBuffers)
    {
        MMS_IL_PRINT("Port buffers already allocated - leaving without allocating new ones\n");
        return OMX_ErrorNone;
    }

    pCompPort->pComponentBuffers = (COMXCompBufferDesc*)TIMM_OSAL_MallocExtn(sizeof(COMXCompBufferDesc) * tPortDef.nBufferCountActual, TIMM_OSAL_TRUE, 32, TIMMOSAL_MEM_SEGMENT_EXT, NULL);

    if (!pCompPort->pComponentBuffers)
    {
        MMS_IL_PRINT("No memory resource for buffer header structure\n");
        return OMX_ErrorInsufficientResources;
    }

    pCompPort->nBufferCount = tPortDef.nBufferCountActual;

    // Set linked component buffers to NULL - there is no linked component yet
    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        pCompPort->pComponentBuffers[bufIdx].pCorespBuffer = NULL;
        pCompPort->pComponentBuffers[bufIdx].pActualPtr = NULL;
        pCompPort->pComponentBuffers[bufIdx].bFree = TIMM_OSAL_TRUE;
    }

    if (nMemMarshallingListSz < nBuffCnt)
    {
        MMS_IL_PRINT("Not enough memory allocated for specified component port\n");
        return OMX_ErrorInsufficientResources;
    }

    for (ii = 0; ii < nBuffCnt; ii++)
    {
        for (i = 0; i < nMemMarshallingListSz; i++)
        {
            if (pMemMarshalling[i].pA9 == ppBuffs[ii])
            {
                break;
            }
        }
        if (i == nMemMarshallingListSz)
        {
            MMS_IL_PRINT("%p specified address not present in component marshalling context list\n", ppBuffs[ii]);
            return OMX_ErrorBadParameter;
        }
    }

    for (bufIdx = 0; bufIdx < tPortDef.nBufferCountActual; bufIdx++)
    {
        MMS_IL_PRINT("Setting allocated %p buffer to OMX component port %u\n", ppBuffs[bufIdx], nPort);
        eError = OMX_UseBuffer(pCompHandle, &(pCompPort->pComponentBuffers[bufIdx].pBuffer), nPort, NULL, tPortDef.nBufferSize, ppBuffs[bufIdx]);
        pCompPort->pComponentBuffers[bufIdx].pActualPtr = ppBuffs[bufIdx];
        if (eError != OMX_ErrorNone)
        {
            MMS_IL_PRINT("OMX Component returned error on allocating buffer resource on port %u buffer index %u\n", nPort, bufIdx);
            return eError;
        }
    }

    return eError;
}

OMX_OTHER_EXTRADATATYPE *COMXComponent::getExtradata(const OMX_PTR ptrPrivate, OMX_EXTRADATATYPE type)
{
    if ( NULL != ptrPrivate )
    {
        const OMX_TI_PLATFORMPRIVATE *platformPrivate = (const OMX_TI_PLATFORMPRIVATE *) ptrPrivate;

        if ( sizeof(OMX_TI_PLATFORMPRIVATE) == platformPrivate->nSize )
        {
            if ( 0 < platformPrivate->nMetaDataSize )
            {
                OMX_U32 remainingSize = platformPrivate->nMetaDataSize;
                OMX_OTHER_EXTRADATATYPE *extraData = (OMX_OTHER_EXTRADATATYPE *) platformPrivate->pMetaDataBuffer;
                if ( NULL != extraData )
                {
                    while ( extraData->eType && extraData->nDataSize && extraData->data &&
                        (remainingSize >= extraData->nSize))
                    {
                        if ( type == extraData->eType ){
                            MMS_IL_PRINT("Data is found - %p!\n", extraData);
                            return extraData;
                        }
                        remainingSize -= extraData->nSize;
                        extraData = (OMX_OTHER_EXTRADATATYPE*) ((char*)extraData + extraData->nSize);
                    }
                } else
                {
                    MMS_IL_PRINT("OMX_TI_PLATFORMPRIVATE pMetaDataBuffer is NULL\n");
                }
            } else
            {
                MMS_IL_PRINT("OMX_TI_PLATFORMPRIVATE nMetaDataSize is size is %d\n",
                             ( unsigned int ) platformPrivate->nMetaDataSize);
            }
        } else
        {
            MMS_IL_PRINT("OMX_TI_PLATFORMPRIVATE size mismatch: expected = %d, received = %d\n",
                         ( unsigned int ) sizeof(OMX_TI_PLATFORMPRIVATE),
                         ( unsigned int ) platformPrivate->nSize);
        }
    }  else
    {
        MMS_IL_PRINT("Invalid OMX_TI_PLATFORMPRIVATE\n");
    }
    MMS_IL_PRINT("Required extradata type wasn't found!\n");
    // Required extradata type wasn't found
    return NULL;
}
