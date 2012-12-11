#ifndef __CDisplay__H__
#define __CDisplay__H__
#include <timm_osal_interfaces.h>
#include <timm_osal_task.h>
#include <timm_osal_trace.h>

typedef struct Rect
{
    int left;
    int right;
    int width;
    int height;
}Rect;
/*
typedef struct DisplayConfig
{
    int globalAlpha;
    int rotation;
    int mirror;
    unsigned char zOrder;
	unsigned char autoAdjustWindow;
    Rect window;
}DisplayConfig;
*/
typedef struct DisplayConfig
{
    TIMM_OSAL_U32 disp_Forced;
    TIMM_OSAL_U32 disp_overlay;
    TIMM_OSAL_U32 disp_Zorder;
    TIMM_OSAL_U32 disp_start_x;
    TIMM_OSAL_U32 disp_start_y;
    TIMM_OSAL_U32 disp_w;  // set some big number. It will be clipped up to display capabilities
    TIMM_OSAL_U32 disp_h;  // set some big number. It will be clipped up to display capabilities
    TIMM_OSAL_U32 autoAdjustWindow;
    TIMM_OSAL_U32 mirror;
    TIMM_OSAL_U32 rotation;
    TIMM_OSAL_U32 globalAlpha;
}DisplayConfig;

typedef struct PerFrameConfiguration
{
    unsigned int format;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
}PerFrameConfiguration;

class CDisplay
{
public:
    virtual TIMM_OSAL_ERRORTYPE Init(DisplayConfig* cfg) = 0;
    virtual void Deinit() = 0;
    virtual void Config(DisplayConfig* cfg) = 0;
    virtual void GetConfig(DisplayConfig* cfg) = 0;
    virtual int PerFrameConfig(PerFrameConfiguration cfg) = 0;
    virtual void* DisplayFrame(void* addrY, void* addrUV, int cropX, int cropY, int cropWidth, int cropHeight) = 0;
    virtual ~CDisplay(){};
};

#endif