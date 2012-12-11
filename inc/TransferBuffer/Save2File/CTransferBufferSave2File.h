#ifndef __CTransferBufferSave2File__H__
#define __CTransferBufferSave2File__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
#include "inc/TransferBuffer/CTransferBuffer.h"

#define FILE_SAVE_PATH      "/mnt/sdcard/test"

class CTransferBufferSave2File : public CTransferBuffer
{
public:
    CTransferBufferSave2File();
    TIMM_OSAL_ERRORTYPE Init(COMXComponent* pComp, int associatedPort, int totalDataSize,
                            void* pServiceHeaderSaveBuffer);
    TIMM_OSAL_ERRORTYPE Transfer(void* dataPtr);

private:
    FILE* fdCapImg;
    dataTransferMessageSave2File_t msg;
    int capture_count;
};

#endif