#ifndef __COMXPortServiceSaveBuffer__H__
#define __COMXPortServiceSaveBuffer__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
#include "inc/TransferBuffer/Save2File/CTransferBufferSave2File.h"
class COMXPortServiceSaveBuffer: public COMXPortService
{
public:
	COMXPortServiceSaveBuffer(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort , void* pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();

private:
	FrmData_t* FrmData;
    ServiceHeaderSaveBuffer_t ServiceHeaderSaveBuffer;
	unsigned int szPtrBuf1;
	unsigned int szPtrBuf2;
    int capture_count;
    CTransferBufferSave2File* SaveBuffer;
    COMXPortHandle* portData;
    bool bIsSnap;
};

#endif