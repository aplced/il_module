#ifndef __COMXPortService__H__
#define __COMXPortService__H__

#include <OMX_Core.h>
#include "inc/lib_comm.h"
#include "inc/MemoryAllocator/CMemoryAllocatorFactory.h"
#include <semaphore.h>

class COMXComponent;

class COMXPortService
{
public:
	COMXPortService(int id);
	virtual ~COMXPortService();
	virtual TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
	virtual TIMM_OSAL_ERRORTYPE Deinit();
	virtual TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType) = 0;
	virtual TIMM_OSAL_ERRORTYPE ConfigPortService() = 0;
    void PopulateConfig(void *ipConfig, int size);
    bool CheckActivity() { return serviceStopping; }
    int ServiceId();
	unsigned int Port();
	void Enable();
	void Disable();
private:
	int serviceId;
	sem_t mutex;
	bool serviceStopping;
protected:
	COMXComponent* pComp;
	unsigned int associatedPort;
	void GetMemoryPtrs(memAllocStrat_t eAllocType, OMX_BUFFERHEADERTYPE* pBufHdr, void** bufPtr1, int* buf1Stride, void** bufPtr2, int* buf2Stride);
	bool StartExecute();
	void StopExecute();
    void* pConfigData;
};

#endif