#include "inc/PortService/Implementation/COMXPortServiceFrameRate.h"

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameRate::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
		return err;
	}

	fRateData = (FrameRateCalcData_t*)malloc(sizeof(FrameRateCalcData_t));

    if(fRateData == NULL)
	{
        printf("FPS: service initilization failed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }


    memset(fRateData->tAbsTime, 0, sizeof(long long)*FRATE_HIST_L);
    fRateData->msrtStart = 0;
    fRateData->totalFrms = 0;
    fRateData->crrT = 0;

    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameRate::Deinit()
{
    if(fRateData == NULL)
    {
        printf("FPS: service not initilized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    free(fRateData);
	fRateData = NULL;

    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameRate::ConfigPortService()
{
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceFrameRate::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
     eAllocType = eAllocType; //stop warning: unused parameter

	if(!StartExecute())
	{
        printf("FrameRate: service not STARTED!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    struct timeval tKDP;

    if(0 != gettimeofday(((struct timeval *)(&tKDP)), NULL))
	{
        printf("FPS: gettimeofday() returned error!\n");
		StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if(fRateData == NULL)
	{
        printf("FPS: service not initialized!\n");
		StopExecute();
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    fRateData->totalFrms++;

    fRateData->tAbsTime[fRateData->crrT] = tKDP.tv_sec*1000000 + tKDP.tv_usec;

    if(fRateData->msrtStart == 1)
	{

        float delta = (fRateData->tAbsTime[fRateData->crrT] - fRateData->tAbsTime[(fRateData->crrT + 1)%FRATE_HIST_L])/1000000.0;

        if(delta >= 6 || ((fRateData->totalFrms % FRATE_HIST_L) == 0))
		{
            printf("\n\t\t*****\tCOMP: %d\tPORT: %d\t%f FPS\t*****\n", pComp->nComponentId, (int)pBufHdr->nOutputPortIndex,((FRATE_HIST_L - 1) / delta));
        }
    }

    if(++(fRateData->crrT) == FRATE_HIST_L)
	{
        fRateData->msrtStart = 1;
        fRateData->crrT = 0;
    }

	StopExecute();
    return TIMM_OSAL_ERR_NONE;
}