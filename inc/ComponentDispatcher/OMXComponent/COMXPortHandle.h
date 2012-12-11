#ifndef __COMXPortHandle__H__
#define __COMXPortHandle__H__

#include <OMX_Core.h>
#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXCompBufferDesc.h"
#define CAPTURE_PORT 5
#define PREVIEW_PORT 2

class COMXPortHandle
{
public:
    unsigned int nPortNo;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

    int nDisplayId;
    bool bAutoBufferCycle;

    memAllocStrat_t eAllocType;
    bool bSupplierPort;

    OMX_ERRORTYPE (*pPortCallback)(COMXPortHandle*, OMX_BUFFERHEADERTYPE*);

    void* pLinkedComponent;

    COMXPortHandle* pLinkedPort;
    COMXCompBufferDesc* pComponentBuffers;
    unsigned int nBufferCount;
};

#endif