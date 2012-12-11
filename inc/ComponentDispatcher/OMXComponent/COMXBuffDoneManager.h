#ifndef __COMXBuffDoneManager__H__
#define __COMXBuffDoneManager__H__

#include <pthread.h>
#include <semaphore.h>
#include <OMX_Core.h>
#include "inc/lib_comm.h"

typedef enum omxBufDoneCBType 
{
    OMX_FILL,
    OMX_EMPTY
}omxBufDoneCBType_t;

typedef struct 
{
    OMX_BUFFERHEADERTYPE* pBuffer;
    omxBufDoneCBType_t cbType;
} OMXbufferDone_t;

class COMXComponent;
class COMXPortHandle;

class COMXBuffDoneManager
{
public:
	COMXBuffDoneManager(COMXComponent* iOMXComp);
	~COMXBuffDoneManager();
	
	TIMM_OSAL_ERRORTYPE Start();
	void Stop();
	
	void Push(omxBufDoneCBType_t cbType, OMX_BUFFERHEADERTYPE* buffer);
	OMXbufferDone_t* Pop();

	COMXComponent* GetOMXComp();
	bool ThreadExit();

private:
	COMXComponent* omxComp;

	bool managmentInited;

	int head;
	int tail;
	int size;
	int items;
	OMXbufferDone_t* pBufDone;
	TIMM_OSAL_ERRORTYPE GrowBuffDoneCircularBuffer();

	bool thExit;
	pthread_t thread;
	sem_t wait4Buffer;
	sem_t mutex;
};

#endif