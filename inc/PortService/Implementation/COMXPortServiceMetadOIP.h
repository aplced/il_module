#ifndef __COMXPortServiceMetadOIP__H__
#define __COMXPortServiceMetadOIP__H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

class COMXPortServiceMetadOIP : public COMXPortService
{
public:
    COMXPortServiceMetadOIP(int id):COMXPortService(id){};
    TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
    TIMM_OSAL_ERRORTYPE Deinit();
    TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
  //  bool CheckActivity();
private:
    dataTransferDescT* outDesc;
};

#endif