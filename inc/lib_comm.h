/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file lib_comm.h
*
* Communication Layer library definitions
*
* @path WTSD_DucatiMMSW/framework/communication_layer
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- */

#ifndef __LIB_COMM_H__
#define __LIB_COMM_H__

//#define DVP_ENABLE

// version : 2 bytes (m)ajor, 1 byte m(i)nor, 1 byte (r)evision
// version : 0xmmmmiirr
#define VER 0x00050302

#define META_OVER_IP_DEFAULT_ADDR   "192.168.0.1"
#define META_OVER_IP_DEFAULT_PORT   7777
#define FRAME_OVER_IP_DEFAULT_ADDR  "172.16.0.1"
#define FRAME_OVER_IP_DEFAULT_PORT  6666

#define FRAME_OVER_IP_CONFIG_FILE  "/mnt/sdcard/mms_il_cfg_frameoverip.txt"
#define META_OVER_IP_CONFIG_FILE   "/mnt/sdcard/mms_il_cfg_metaoverip.txt"
#define FILE_SAVE_PATH             "/mnt/sdcard/test"

#define COMM_TCPIP_BUILD
// #define COMM_STANDALONE_BUILD

/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <termios.h>
#include <poll.h>

#include <linux/ion.h>
#include <linux/omap_ion.h>

#include <timm_osal_interfaces.h>
#include <timm_osal_task.h>
#include <timm_osal_trace.h>

#include <OMX_Core.h>
#include <OMX_CoreExt.h>
#include <OMX_Index.h>
#include <OMX_IndexExt.h>
#include <OMX_TI_Index.h>
#include <OMX_Component.h>
#include <OMX_IVCommon.h>
#include <OMX_TI_IVCommon.h>
#include <OMX_TI_Common.h>

#ifdef DVP_ENABLE
#include <dvp/dvp.h>
#include <dvp/dvp_debug.h>
#endif

#include "inc/Display/CDisplay.h"
#include "inc/Communication/Channel/CCommunicationChannel.h"
#include "inc/Communication/Channel/Implementation/CCommunicationChannelImplementation.h"

/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#if 1
#define MMS_IL_PRINT(ARGS...) { printf("        %s:%s(%d)\n", __FILE__, __FUNCTION__, __LINE__); printf(ARGS); }
#else
#define MMS_IL_PRINT(ARGS...)
#endif

#define ALIGN(x, a)	(((x) + (a) - 1L) & ~((a) - 1L))


#define OMX_INIT_STRUCT_PTR(_s_, _name_)        \
    memset((_s_), 0x0, sizeof(_name_));         \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision = 0x0;          \
    (_s_)->nVersion.s.nStep = 0x0


typedef enum
{
    MEM_OMX,
    MEM_OSAL,
    MEM_ION_1D,
    MEM_ION_2D,
    MEM_UNDEF
} memAllocStrat_t;

/**
 * Config message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nCommand;
    TIMM_OSAL_U32 nParam;
    TIMM_OSAL_PTR pCommandData;
} commandMessage_t;

/**
 * Config message payload structure
 */
typedef struct
{
    unsigned int nComponentId;
    unsigned int nStructureId;
    unsigned int nStructureSize;
    unsigned int nPortNumber;
    unsigned char pConfigData[1];
} configMessage_t;

/**
 * Get handle message payload structure
 */
typedef struct
{
    TIMM_OSAL_CHAR cComponentName[1];
} getHandleMessage_t;

/**
 * Get handle message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nCompId;
    TIMM_OSAL_CHAR cComponentName[1];
} getHandleMessageAck_t;

/**
 * Release handle message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
} releaseHandleMessage_t;

/**
 * Data message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32  nDataId;
    TIMM_OSAL_CHAR cDataBytes[1];
} dataMessage_t;

/**
 * Link components message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponent1Id;
    TIMM_OSAL_U32 nComponent1Port;
    TIMM_OSAL_U32 nComponent2Id;
    TIMM_OSAL_U32 nComponent2Port;
} linkMessage_t;

/**
 * Allocate port buffers message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nBuffNumber;
} allocateMessage_t;

/**
 * Allocate port buffers message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nExtraBufferCount;
} allocateExtraMessage_t;

/**
 * Get component ports message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
} getportsMessage_t;

typedef struct
{
    TIMM_OSAL_U32 nPortNo;
    OMX_DIRTYPE eDir;
    OMX_PORTDOMAINTYPE eDomain;
} portData_t;

/**
 * Get component ports message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    portData_t    pPortData[1];
} getportsMessageAck_t;

/**
 * Set up data streamer
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nDataType;
    TIMM_OSAL_U32 nMsgId;
    TIMM_OSAL_U32 nTransportType;
    TIMM_OSAL_CHAR cFileExt[1];
} dataTransferSetUp_t;

/**
 * Set up data streamer
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nDispId;
    DisplayConfig Config;
} displaySetUp_t;

typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    DisplayConfig Config;
} displayConfig_t;
/**
 * Set up port buffer allocation methods
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nAllocMethod;
} omxPortAllocMethodSetUp_t;

/**
 * Set up marshalling context memory allocation methods
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nAllocMethod;
} marshalMemAllocMethodSetUp_t;

/**
 * Marshalling memory allocation message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32  nComponentId;
    TIMM_OSAL_U32  nSize;
    TIMM_OSAL_PTR  nPtr;
    TIMM_OSAL_U32  nOffset;
} marshalMemAllocMessage_t;

/**
 * Marshalling file copying message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32  nComponentId;
    TIMM_OSAL_U32  nPortNum;
    TIMM_OSAL_U32  nBuffNum;
    TIMM_OSAL_U32  nSize;
    TIMM_OSAL_CHAR cFileName[1];
} marshalFileCopyMessage_t;

/**
 * Use marshalled memory on port message payload structure
 */
typedef struct
{
    TIMM_OSAL_U32   nComponentId;
    TIMM_OSAL_U32   nPortNumber;
    TIMM_OSAL_U32   nBufNumber;
    TIMM_OSAL_U8*   nBufPtr;
} marshalMemUseOnPortMessage_t;

/**
 * Memory for marshalling descriptor
 */
typedef struct
{
    TIMM_OSAL_PTR pA9;
} marshalMem_t;

/**
 * Set up port services
 */
typedef struct
{
    TIMM_OSAL_U32  nComponentId;
    TIMM_OSAL_U32  nPortNumber;
    TIMM_OSAL_U32  nServiceType;
    TIMM_OSAL_U8 pServiceHeader[1];
} setUpPortSrvcMessage_t;

/**
 * IL module generated events
 */
typedef struct
{
    TIMM_OSAL_U32  nEventId;
    TIMM_OSAL_U32  nEventStatus;
    TIMM_OSAL_U32  nEventData1;
    TIMM_OSAL_U32  nEventData2;
} internEventMessage_t;

/**
 * Auto-buffer managment on specific port
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
} autoBufMngmentMessage_t;

/**
 * IL Client version message
 */
typedef struct
{
    TIMM_OSAL_U32 nVersion;
} ilClientVersionMessage_t;

/**
 * I2C message
 */
typedef struct
{
    TIMM_OSAL_U32 nSubAddr;
    TIMM_OSAL_U32 nPayloadSize;
     TIMM_OSAL_U8 pPayloadData[1];
} I2CData_t;

typedef struct
{
    TIMM_OSAL_U32 nSlaveAddr;
    I2CData_t pData[1];
} I2CMessage_t;

/**
 * Data transfer task messaging pipe (global)
 */
extern TIMM_OSAL_PTR dataTransfer2HostMsgPipe;

/**
 * Media used to transfer the data (messaging, file i/o, ...)
 */
typedef enum
{
    DTM_MSG,
    DTM_FILE_IO,
    DTM_NETWORK,
    DTM_UNDEF
} dataTransferMediaT;


/**
 * OMX Get component version structure
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_S8 cCompName[OMX_MAX_STRINGNAME_SIZE];
    TIMM_OSAL_U32 nCompVersion;
    TIMM_OSAL_U32 nSpecVersion;
    TIMM_OSAL_U8 cCompUUID[OMX_MAX_STRINGNAME_SIZE];
} getOMXVersion_t;

/**
 * Data transferring file name prefix setting
 */
typedef struct
{
    TIMM_OSAL_U32 nComponentId;
    TIMM_OSAL_U32 nPortNumber;
    TIMM_OSAL_U32 nServiceNumber;
    TIMM_OSAL_PTR pConfig[1];
} configPortService_t;

typedef struct
{
    unsigned int nStride;
    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int nBpp;
    unsigned int nFrmCount;
}FrmData_t;

typedef struct
{
    TIMM_OSAL_U32 nInitiatorID;
    TIMM_OSAL_U32 nInitiatorPort;
    TIMM_OSAL_U32 nDataType;
} dataInitiator_t;

//Save2file
typedef struct
{
    dataInitiator_t Initiator;
    TIMM_OSAL_U8* dataBuffPtr;
    TIMM_OSAL_U32 dataSize;
    char namePrfx[128];
    char fileExt[8];
}dataTransferMessageSave2File_t;

typedef struct
{
   char namePrfx[128];
   char fileExt[8];
   char filePath[128];
}ServiceHeaderSaveBuffer_t;

typedef struct
{
   TIMM_OSAL_U32 dataTypeID;
   TIMM_OSAL_U32 nMsgId;
}ServiceHeaderStreamBuffer_t;

//stream2Host
typedef struct
{
    dataInitiator_t Initiator;
    TIMM_OSAL_U8* dataBuffPtr;
   // TIMM_OSAL_U32 transferMsgIdD;
}dataTransferMessageFull_t;

/**
 * Transfer data ownership info
 */
typedef struct
{
    dataInitiator_t Initiator;
    TIMM_OSAL_U32 nTotalDataSize;
} dataTransferMessage_t;
/**
 * DVP Messages
 */
#ifdef DVP_ENABLE
typedef struct {
    DVP_U32   nComponentID;
    DVP_U32   nImage;
    fourcc_t  color;
} DVP_ImageInitMessage_t;

typedef struct {
    DVP_U32       nImage;
    fourcc_t      color;
    DVP_MemType_e type;
} DVP_Image_Params_t;

typedef struct {
    DVP_U32 nComponentID;
    uint32_t width;
    uint32_t height;
    uint32_t type;
    uint32_t capacity;
    uint32_t iterations;
    uint32_t numNodes;
    bool_e   serial;
} DVP_InitParamsMessage_t;

typedef struct {
    DVP_U32       nComponentID;
    DVP_U32       nImage;
    DVP_MemType_e type;
} DVP_ImageAllocMessage_t;

typedef struct {
    DVP_U32 nComponentID;
    DVP_U32 len;
    void* ptr;
} DVP_ImageDupMessage_t;

typedef struct {
    DVP_U32 nComponentID;
    DVP_U32 numParams;
    DVP_Image_Params_t params[1];
} DVP_ImageConstructMessage_t;

typedef struct {
    DVP_U32 nComponentID;
    DVP_Core_e nCore;
    DVP_U32 capacity;
} DVP_CoreCapacityMessage_t;

typedef struct {
    DVP_U32 nComponentID;
    DVP_U32 nNodes;
} DVP_KAlloc_Message_t;

typedef struct {
    DVP_U32 nComponentID;
    DVP_U32 nNode;      //number of new to be initialized
    DVP_U32 nInput;     //input image
    DVP_U32 nOutput;    //output image
    DVP_U32 kernel;
    DVP_U32 affinity;
} DVP_InitNodeMessage_t;
#endif

/**
 * Data transfer descriptor
 */
typedef struct dataTransferDescT__ dataTransferDescT;

/**
 * Data transfer done callback
 */
typedef  void (*dataTransferDoneCallback) (dataTransferDescT* outDesc);

/**
 * Data transfer descriptor
 */
struct dataTransferDescT__
{
    TIMM_OSAL_U32 initiatorID;
    TIMM_OSAL_U32 initiatorPort;

    TIMM_OSAL_U8* dataBuffPtr;
    TIMM_OSAL_U32 dataOffset;
    TIMM_OSAL_U32 dataSize;
    TIMM_OSAL_U32 dataWidth;
    TIMM_OSAL_U32 dataStride;
    TIMM_OSAL_U32 dataTypeID;
    char namePrfx[128];
    char fileExt[8];

    TIMM_OSAL_U32 transferMsgIdD;
    dataTransferMediaT tranfMedia;

    TIMM_OSAL_PTR customData;
    dataTransferDoneCallback OnTransferDone;
};
TIMM_OSAL_U32 copyFile2Buffer(char* pFileName, TIMM_OSAL_PTR pDestBuff, TIMM_OSAL_U32 nCpySz);

#endif // __LIB_COMM_H__
