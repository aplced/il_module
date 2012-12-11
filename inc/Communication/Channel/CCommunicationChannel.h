#ifndef __CCommunicationChannel__H__
#define __CCommunicationChannel__H__

#include "inc/Communication/Media/CCommunicationMedia.h"

class CCommunicationChannel
{
public:
    virtual TIMM_OSAL_ERRORTYPE Init() = 0;
    virtual void Deinit() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void WaitOnCommObject() = 0;
    virtual TIMM_OSAL_ERRORTYPE Write(systemMessage_t* message) = 0;
    virtual ~CCommunicationChannel(){};

    bool isClosing() { return Exit; }

protected:

    bool Exit;

};

#endif