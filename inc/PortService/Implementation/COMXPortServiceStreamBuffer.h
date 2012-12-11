#ifndef __COMXPortServiceStreamBuffer__H__
#define __COMXPortServiceStreamBuffer__H__

//#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
#include "inc/TransferBuffer/Stream2Host/CTransferBufferStream2Host.h"
//#include <semaphore.h>

class COMXPortServiceStreamBuffer: public COMXPortService
{
	friend void* frameTransferThread_func(void* pArgv);

public:
	COMXPortServiceStreamBuffer(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	//bool CheckActivity();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
private:
	FrmData_t* FrmData;
	unsigned int szPtrBuf1;
	unsigned int szPtrBuf2;
	//void InitDataTransfer(int totalDataSize, int compId, int portNum);
	//void TransferLine(void* dataPtr, int dataSize, int status);
	//void TransferData(void* dataPtr, int dataSize);
	sem_t frameSendMutex;
    COMXPortHandle* portData;
    ServiceHeaderStreamBuffer_t* pServiceHeaderStreamBuffer;
};

#endif