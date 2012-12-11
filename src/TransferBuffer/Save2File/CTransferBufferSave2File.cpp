#include "inc/TransferBuffer/Save2File/CTransferBufferSave2File.h"

CTransferBufferSave2File::CTransferBufferSave2File()
{
    capture_count = 1;
}

TIMM_OSAL_ERRORTYPE CTransferBufferSave2File::Init(COMXComponent* pComp, int associatedPort, int totalDataSize, void* pServiceHeaderSaveBuffer)
{
    static char file_name[255];
    char path[128];

    strcpy(msg.namePrfx, ((ServiceHeaderSaveBuffer_t*)pServiceHeaderSaveBuffer)->namePrfx);
    strcpy(msg.fileExt,((ServiceHeaderSaveBuffer_t*)pServiceHeaderSaveBuffer)->fileExt);
    //strcpy(msg.filePath,((ServiceHeaderSaveBuffer_t*)pServiceHeaderSaveBuffer)->filePath);
    int len1 = strlen((const char *)msg.namePrfx);
    int len2 = strlen((const char *)msg.fileExt);
    int len3 = strlen((const char *)((ServiceHeaderSaveBuffer_t*)pServiceHeaderSaveBuffer)->filePath);
    strcpy(path, (len3)?((ServiceHeaderSaveBuffer_t*)pServiceHeaderSaveBuffer)->filePath:FILE_SAVE_PATH);
    //MMS_IL_PRINT("namePrfx - %s\n", msg.namePrfx);
    // MMS_IL_PRINT("fileExt - %s\n",msg.fileExt);
    if (len1>0) strcat(msg.namePrfx, "_");
    if (len2>0)
    {
       char str[9]= ".";
       strcat(str, msg.fileExt);
       strcpy(msg.fileExt, str);
    }
    msg.Initiator.nInitiatorPort = associatedPort;
    msg.Initiator.nInitiatorID = pComp->nComponentId;
    msg.dataSize = totalDataSize;

/* Adjust index so existing files are not overwritten */
    while (1)
    {//Check whether file exists, loop until nonexisting file name is found
        sprintf(file_name,
            "%s/%sc%02dp%02d_%04d%s",
            path, msg.namePrfx, msg.Initiator.nInitiatorID,
            msg.Initiator.nInitiatorPort, capture_count,
            msg.fileExt);

        capture_count++;
        fdCapImg = fopen(file_name, "rb");

        if (!fdCapImg)
        {
            break;
        }
        fclose(fdCapImg);

    }
   //  MMS_IL_PRINT("file_name - %s\n",file_name);
    //Open image for storage
    fdCapImg = fopen(file_name, "wb");
    if (!fdCapImg)
    {
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        fdCapImg = fopen(file_name, "wb");
    }
    if (!fdCapImg)
    {
       MMS_IL_PRINT("Cannot open file: %s\n", file_name);
       return TIMM_OSAL_ERR_UNKNOWN;
    }
    MMS_IL_PRINT("File %s is opened\n",file_name);
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CTransferBufferSave2File::Transfer(void* dataPtr)
{
    unsigned int bytes_written = 0;

    msg.dataBuffPtr = (TIMM_OSAL_U8*)dataPtr;

        bytes_written = fwrite(dataPtr, 1, msg.dataSize, fdCapImg);
        if (bytes_written != msg.dataSize)
		{
            printf("Written size mismatch: requested - %d, written - %d\n", msg.dataSize, bytes_written);
            fclose(fdCapImg);
            return TIMM_OSAL_ERR_MSG_SIZE_MISMATCH;
        }
     MMS_IL_PRINT("%d bytes are written to file\n", bytes_written);
     fclose(fdCapImg);
     return TIMM_OSAL_ERR_NONE;
     //printf("File %s written\n", file_name);
}
