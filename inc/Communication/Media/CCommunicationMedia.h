#ifndef __CCommunicationMedia__H__
#define __CCommunicationMedia__H__
#include <timm_osal_interfaces.h>
#include <timm_osal_task.h>
#include <timm_osal_trace.h>

#define COMM_TCPIP_BUILD

/* Maximum message payload buffer */
#define MSG_MAX_PAYLOAD     (640*480*4)

/**
 * Message type
 */
typedef struct {
    unsigned int nId;          /** id of the message */
    unsigned int nStatus;          /** message status*/
    unsigned int nPayloadSize;     /** message payload size in bytes */
    void* pPayload;         /** pointer to payload */
} systemMessage_t;

class CCommunicationMedia
{
public:
    virtual TIMM_OSAL_ERRORTYPE Init() = 0;
    virtual TIMM_OSAL_ERRORTYPE Deinit() = 0;
    virtual TIMM_OSAL_ERRORTYPE Receive(void* pCurMessage) = 0;
    virtual TIMM_OSAL_ERRORTYPE Send(void* pMessage) = 0;
    virtual ~CCommunicationMedia(){};
};

#endif