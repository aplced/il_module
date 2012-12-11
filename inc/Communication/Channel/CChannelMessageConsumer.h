#ifndef __CChannelMessageConsumer__H__
#define __CChannelMessageConsumer__H__

class CChannelMessageConsumer
{
public:
    //Call process function for the given message handle
    virtual TIMM_OSAL_ERRORTYPE MessageEventHandler(void* pMessage) = 0;
    virtual ~CChannelMessageConsumer(){};
};

#endif