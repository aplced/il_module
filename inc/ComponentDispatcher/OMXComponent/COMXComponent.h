#ifndef __COMXComponent__H__
#define __COMXComponent__H__

#include <stdlib.h>
#include <map>

#include <pthread.h>
#include <semaphore.h>
#include <OMX_Core.h>
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"
#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/COMXComponentDispatcher.h"
#include "inc/Display/CDisplay.h"
#include "inc/PortService/COMXPortService.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXBuffDoneManager.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXPortHandle.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXCompBufferDesc.h"

using namespace std;

#define PORT_FINAL (0xFFFFFFFF)

#define COMPONENT_EVENT_CMD_STATE_CHANGE (1 << 0)
#define COMPONENT_EVENT_CMD_PORT_DISABLE (1 << 1)
#define COMPONENT_EVENT_CMD_PORT_ENABLE (1 << 2)

#define STRUCT_ACQ_ARRAY_SZ (5)

OMX_ERRORTYPE OutputPortCallback(COMXPortHandle *pPortLink, OMX_BUFFERHEADERTYPE *pBuffer);
OMX_ERRORTYPE InputPortCallback(COMXPortHandle *pPortLink, OMX_BUFFERHEADERTYPE *pBuffer);

class COMXPortService;
class COMXComponentDispatcher;

class COMXComponent
{
public:
    COMXComponent(COMXComponentDispatcher* hDispatcher, char* iCompName);
    ~COMXComponent();

    char* compName;
	unsigned int nComponentId;
	void* OMXCompHdl(){return pCompHandle;};
	COMXComponentDispatcher* dispatcher;
    CDisplay* displayModule;
     //Execute OMX_GetHandle
	OMX_ERRORTYPE GetOMXCompHdl();
	OMX_ERRORTYPE GetComponentVersion(OMX_VERSIONTYPE* compVersion, OMX_VERSIONTYPE* specVersion, OMX_UUIDTYPE* copmUUID);

	OMX_ERRORTYPE AllocatePortBuffers(TIMM_OSAL_U32 nPort);
	OMX_ERRORTYPE FreeOMXBuffer(OMX_BUFFERHEADERTYPE* pBuffer, int nPort);
	OMX_ERRORTYPE FreePortBuffers(TIMM_OSAL_U32 nPort);
	OMX_ERRORTYPE FreeAllPortBuffers();

	OMX_ERRORTYPE LinkPortBuffers(TIMM_OSAL_U32 nPort);
	OMX_ERRORTYPE UseMarshalledMemOnPortBuffers(unsigned int nPort, unsigned int nBuffCnt, unsigned char** ppBuffs);

	OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
    OMX_ERRORTYPE QueueFillPortBuffers(OMX_U32 nPort , OMX_U32 bufIdx);
	OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
    OMX_ERRORTYPE QueueEmptyPortBuffers(OMX_U32 nPort , OMX_U32 bufIdx, bool bSend2linkedComp);

	OMX_ERRORTYPE SetParam(OMX_INDEXTYPE structId, unsigned char* dataPtr);
	OMX_ERRORTYPE GetParam(OMX_INDEXTYPE structId, unsigned char* dataPtr);
	COMXPortHandle* GetPortData(OMX_U32 nPort);
	OMX_ERRORTYPE GetOMXPortDef(OMX_PARAM_PORTDEFINITIONTYPE* tPortDef, int nPort);

    OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE structId, unsigned char* dataPtr);
	OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE structId, unsigned char* dataPtr);
    OMX_ERRORTYPE Command(void* pCommandMessage, void* pComponentData);
    OMX_ERRORTYPE PortDisable(void* pComponent, unsigned int nPort);
    OMX_ERRORTYPE PortEnable(unsigned int nPort);

	OMX_ERRORTYPE AddPortSevice(int portNum, int serviceType, void*, int nServiceHeadSize);
	OMX_ERRORTYPE RemovePortService(unsigned int portNum, int serviceType);
    OMX_OTHER_EXTRADATATYPE *getExtradata(const OMX_PTR ptrPrivate, OMX_EXTRADATATYPE type);
	void RunPortServices(COMXPortHandle* pCompPort, OMX_BUFFERHEADERTYPE* pBufHdr);
    void ConfigPortServices(configPortService_t* pConfig, int size);
    void ConfigAllPortServices();
	int VideoPortCount(){return videoPortCount;};
	int AudioPortCount(){return audioPortCount;};
	int ImagePortCount(){return imagePortCount;};
	int OtherPortCount(){return otherPortCount;};

	int VideoPortStartIndex(){return videoPortStartIndex;};
	int AudioPortStartIndex(){return audioPortStartIndex;};
	int ImagePortStartIndex(){return imagePortStartIndex;};
	int OtherPortStartIndex(){return otherPortStartIndex;};
    marshalMem_t* pMemMarshalling;
    unsigned int nMemMarshallingListSz;
    memAllocStrat_t tMemMarshallingAllocStrat;

private:
	void* pCompHandle;
	OMX_CALLBACKTYPE* pComponentCallbacks;
	COMXBuffDoneManager* buffMgr;

	TIMM_OSAL_PTR pComponentEvents;
	OMX_STATETYPE ComponentState;
    OMX_STATETYPE LastStateCommand;

	OMX_ERRORTYPE SetStateLoaded();
	OMX_ERRORTYPE SetStateIdle();
	OMX_ERRORTYPE SetStateExecuting();

	OMX_ERRORTYPE WaitForState(OMX_STATETYPE DesiredState);
	OMX_ERRORTYPE WaitForPortDisable();
	OMX_ERRORTYPE WaitForPortEnable();

    friend OMX_ERRORTYPE OMXComponentEventHandler(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
    friend OMX_ERRORTYPE OMXComponentFillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);
    friend OMX_ERRORTYPE OMXComponentEmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer);

	OMX_ERRORTYPE InitializePortData();
	OMX_ERRORTYPE GetOMXInitPortData(OMX_INDEXTYPE portInitIndx, int* portCount, int* portStartIndx);
	void InitPortData(int numPorts, int startIndex);

	void DeinitializePortData();
	void ReleasePortResources(int numPorts, int startIndx);

	map<int, COMXPortHandle*> portHdls;

	int videoPortCount;
	int videoPortStartIndex;

	int audioPortCount;
	int audioPortStartIndex;

	int imagePortCount;
	int imagePortStartIndex;

	int otherPortCount;
	int otherPortStartIndex;

	int serviceCount;
	vector<COMXPortService*> portServices;
    sem_t portServiceMutex, state_set_sem, get_port_sem;
};

#endif