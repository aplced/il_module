#include "inc/PortService/COMXPortServiceManager.h"
#include "inc/PortService/Implementation/COMXPortServiceFrameRate.h"
#include "inc/PortService/Implementation/COMXPortServiceExtractMetad.h"
#include "inc/PortService/Implementation/COMXPortServiceFrameOIP.h"
#include "inc/PortService/Implementation/COMXPortServiceFaceRectDraw.h"
#include "inc/PortService/Implementation/COMXPortServiceSaveBuffer.h"
#include "inc/PortService/Implementation/COMXPortServiceMetadOIP.h"
#include "inc/PortService/Implementation/COMXPortServiceStreamBuffer.h"
#include "inc/PortService/Implementation/COMXPortServiceAFStatusCallBack.h"

COMXPortService* COMXPortServiceManager::CreatePortService(int ServiceType)
{
    switch(ServiceType)
    {
        case FRATE_MEASURE_SRVC:
            return new COMXPortServiceFrameRate(FRATE_MEASURE_SRVC);
        case METADATA_ETRC_SRVC:
            return new COMXPortServiceExtractMetad(METADATA_ETRC_SRVC);
        case FRAME_OVER_IP_SRVC:
            return new COMXPortServiceFrameOIP(FRAME_OVER_IP_SRVC);
        case DRAW_FACERECT_SRVC:
            return new COMXPortServiceFaceRectDraw(DRAW_FACERECT_SRVC);
        case SAVE_BUFFER_SRVC:
            return new COMXPortServiceSaveBuffer(SAVE_BUFFER_SRVC);
        case METAD_OVER_IP_SRVC:
            return new COMXPortServiceMetadOIP(METAD_OVER_IP_SRVC);
		case STRM_BUFFER_SRVC:
            return new COMXPortServiceStreamBuffer(STRM_BUFFER_SRVC);
        case AF_CALLBACK_SRVC:
            return new COMXPortServiceAFStatusCallBack(AF_CALLBACK_SRVC);
        default:
            return NULL;
    }
}