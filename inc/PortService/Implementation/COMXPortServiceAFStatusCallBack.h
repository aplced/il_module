#ifndef __COMXPortServiceAFStatusCallBack__H__
#define __COMXPortServiceAFStatusCallBack__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

typedef enum {
    AF_STATUS_RUNNING = 0,
    AF_STATUS_FAIL = 1,
    AF_STATUS_SUCCESS = 2,
    AF_STATUS_SUSPEND = 3,
    AF_STATUS_IDLE = 4
} FOCUS_STATUS_VALUES;

struct AF_Callback_t {
    OMX_S32 AFstat;
    OMX_S32 nLeft;              /**< The leftmost coordinate of the focus region rectangle */
    OMX_S32 nTop;               /**< The topmost coordinate of the focus region rectangle */
    OMX_U32 nWidth;             /**< The width of the focus region rectangle in pixels */
    OMX_U32 nHeight;            /**< The height of the focus region rectangle in pixels */
};

class COMXPortServiceAFStatusCallBack : public COMXPortService
{
public:
	COMXPortServiceAFStatusCallBack(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void*pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
private:
    FOCUS_STATUS_VALUES lastAFStatus;
};

#endif