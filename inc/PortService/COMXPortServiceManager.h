#ifndef __COMXPortServiceManager__H__
#define __COMXPortServiceManager__H__

#include "inc/PortService/COMXPortService.h"

#define FRATE_MEASURE_SRVC 0
#define METADATA_ETRC_SRVC 1
#define FRAME_OVER_IP_SRVC 2
#define DRAW_FACERECT_SRVC 3
#define SAVE_BUFFER_SRVC 4
#define METAD_OVER_IP_SRVC 5
#define STRM_BUFFER_SRVC 6
#define AF_CALLBACK_SRVC 7

class COMXPortServiceManager
{
public:
	COMXPortService* CreatePortService(int ServiceType);
};

#endif