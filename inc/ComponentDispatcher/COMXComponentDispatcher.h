#ifndef __COMXComponentDispatcher__H__
#define __COMXComponentDispatcher__H__

#include <stdlib.h>
#include <map>

#include "inc/messages.h"
#include "inc/lib_comm.h"

#include "inc/Communication/Channel/CCommunicationChannel.h"
#include "inc/Communication/Channel/CChannelMessageConsumer.h"
#include "inc/Communication/Channel/Implementation/CCommunicationChannelImplementation.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

#ifdef DVP_ENABLE
#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"
#endif

//#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"
#include "inc/Display/CDisplay.h"
#include "inc/PortService/COMXPortServiceManager.h"

typedef struct
{
    unsigned int  nEventId;
    unsigned int  nEventStatus;
    unsigned int  nEventData1;
    unsigned int  nEventData2;
} InternEventMessage;

using namespace std;

class CComponentDispatcherMessageHandle;

class COMXComponentDispatcher: public CChannelMessageConsumer
{
public:
    COMXComponentDispatcher();
    void Start();
    void GenerateInternalEvent(InternEventMessage eventData);
    void CloseChannel();
    void RemoveOMXComponentSlot(COMXComponent* delComp);

    COMXComponent* GetOMXComponentSlot(unsigned int releaseCompHdl);
    COMXComponent* AddOMXComponentSlot(void* copmName);
    TIMM_OSAL_ERRORTYPE SendMessage(systemMessage_t* outMsg);
    TIMM_OSAL_ERRORTYPE MessageEventHandler(void* pMessage);
#ifdef DVP_ENABLE
    void RemoveDVPComponentSlot(CDVPComponent* delComp);
    CDVPComponent* GetDVPComponentSlot(unsigned int ID);
    CDVPComponent* AddDVPComponentSlot();
#endif
    int I2C_device;//I2C file descriptor
	//CDisplay* displayModule;
	COMXPortServiceManager portServices;

private:
	map<int, COMXComponent*> msgIdComponentTable;
#ifdef DVP_ENABLE
    map<int, CDVPComponent*> DVPComponentTable;
#endif
	void AddMessageHandleToHold(int msgId);
	map<int, CComponentDispatcherMessageHandle*> msgHandlesHold;
    CCommunicationChannel* pCommChan;

	CComponentDispatcherMessageHandle* GetMsgHandler(unsigned int id);
};

#endif