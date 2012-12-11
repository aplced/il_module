#ifndef __COMXPortServiceFrameOIP__H__
#define __COMXPortServiceFrameOIP__H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

typedef struct {
    unsigned int nSocket;
    unsigned int nStride;
    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int nBpp;
    unsigned int nFrmCount;
    unsigned int nBuffSize;
    unsigned int nNumFrames;
    unsigned int nNumFramesSent;
    pthread_t FrameSendThread;
    sem_t semSend;
    sem_t semThreadFree;
    sem_t semExit;
    char *thBuff;
} FrmOvrIPData_t;

typedef struct{
    OMX_U32   byteSize;           //Total buffer size (frame and metadata excluding header size(this struct))
    OMX_TICKS timeStamp;          //Associate frame time stamp
    OMX_U32   validDataOffset;    //Offset in bytes of frame data from buffer start
    OMX_U32   frameSize;          //Size of valid frame data
    OMX_U32   validMetaOffset;    //Offset in bytes of meta data from buffer start
    OMX_U32   metaSize;           //Size of valid meta data
}PrvOutgoingFrmHdr_t;

class COMXPortServiceFrameOIP : public COMXPortService
{
public:
	COMXPortServiceFrameOIP(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
   // bool CheckActivity();
private:
	FrmOvrIPData_t* prvOvrIPData;
	int framePrintOut;
};

#endif