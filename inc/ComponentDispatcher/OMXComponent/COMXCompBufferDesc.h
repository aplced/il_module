#ifndef __COMXCompBufferDesc__H__
#define __COMXCompBufferDesc__H__

#include <OMX_Core.h>

class COMXCompBufferDesc
{
public:
    void* pActualPtr;
    unsigned int bFree;
    OMX_BUFFERHEADERTYPE *pBuffer;
    OMX_BUFFERHEADERTYPE *pCorespBuffer;
};

#endif