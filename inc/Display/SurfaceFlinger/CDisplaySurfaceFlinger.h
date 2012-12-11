#ifndef __CDisplaySurfaceFlinger__H__
#define __CDisplaySurfaceFlinger__H__

#include "inc/Display/CDisplay.h"
#include <OMX_IVCommon.h>

#include <surfaceflinger/Surface.h>
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/ISurfaceComposer.h>
#include <surfaceflinger/ISurfaceComposerClient.h>
#include <surfaceflinger/SurfaceComposerClient.h>

#define MAX_PREVIEW_SURFACE_WIDTH   800
#define MAX_PREVIEW_SURFACE_HEIGHT  480

using namespace android;

class CDisplaySurfaceFlinger : public CDisplay
{
public:
	CDisplaySurfaceFlinger();
    TIMM_OSAL_ERRORTYPE Init(DisplayConfig* cfg);
    void Deinit();
    void Config(DisplayConfig* cfg);
    void GetConfig(DisplayConfig* cfg);
    int PerFrameConfig(PerFrameConfiguration cfg);
    void* DisplayFrame(void* addrY, void* addrUV, int cropX, int cropY, int cropWidth, int cropHeight);

private:
	sp<SurfaceComposerClient> client;
	sp<SurfaceControl> surfaceControl;
	sp<Surface> previewSurface;

	int CreateSurface(unsigned int width, unsigned int height, int pixFormat);
	int DestroySurface();
};

#endif