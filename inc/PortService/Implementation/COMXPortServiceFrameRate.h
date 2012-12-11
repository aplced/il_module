#ifndef __COMXPortServiceFrameRate__H__
#define __COMXPortServiceFrameRate__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

#define FRATE_HIST_L 30

typedef struct
{
    long long tAbsTime[FRATE_HIST_L];
    unsigned int msrtStart;
    unsigned int totalFrms;
    unsigned int crrT;
}FrameRateCalcData_t;

class COMXPortServiceFrameRate : public COMXPortService
{
public:
	COMXPortServiceFrameRate(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	//bool CheckActivity();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
private:
	FrameRateCalcData_t* fRateData;
};

#endif