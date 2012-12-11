#ifndef __COMXPortServiceExtractMetad__H__
#define __COMXPortServiceExtractMetad__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
#include "inc/TransferBuffer/Save2File/CTransferBufferSave2File.h"
class COMXPortServiceExtractMetad : public COMXPortService
{
public:
	COMXPortServiceExtractMetad(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void*pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
   // bool CheckActivity();
private:

    ServiceHeaderSaveBuffer_t ServiceHeaderSaveBuffer;
};

#endif