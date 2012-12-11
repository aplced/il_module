#ifndef __CComponentDispatcherMessageHandle__H__
#define __CComponentDispatcherMessageHandle__H__

#include "inc/ComponentDispatcher/COMXComponentDispatcher.h"
#include "inc/lib_comm.h"

class CComponentDispatcherMessageHandle
{
public:
	//initialize protected dispatcher and id
    CComponentDispatcherMessageHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId)
	{dispatcher = hDispatcher; id = hId;};
	//get ID
    unsigned int Id(){return id;};
    virtual TIMM_OSAL_ERRORTYPE Process(void* pMessage) = 0;

protected:
    COMXComponentDispatcher* dispatcher;
    unsigned int id;
    virtual ~CComponentDispatcherMessageHandle() {};
};

#endif