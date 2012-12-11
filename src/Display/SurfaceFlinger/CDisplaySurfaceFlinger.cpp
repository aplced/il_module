#include "inc/Display/SurfaceFlinger/CDisplaySurfaceFlinger.h"
#include "inc/lib_comm.h"

CDisplaySurfaceFlinger::CDisplaySurfaceFlinger()
{
}

TIMM_OSAL_ERRORTYPE CDisplaySurfaceFlinger::Init(DisplayConfig* cfg)
{
    cfg=cfg;
    return 0;
}

void CDisplaySurfaceFlinger::Deinit()
{
}

void CDisplaySurfaceFlinger::Config(DisplayConfig* cfg)
{
}

void CDisplaySurfaceFlinger::GetConfig(DisplayConfig* cfg)
{
}

int CDisplaySurfaceFlinger::PerFrameConfig(PerFrameConfiguration cfg)
{
	return 0;
}

void* CDisplaySurfaceFlinger::DisplayFrame(void* addrY, void* addrUV, int cropX, int cropY, int cropWidth, int cropHeight)
{
	return NULL;
}

int CDisplaySurfaceFlinger::CreateSurface(unsigned int width, unsigned int height, int pixFormat)
{
	unsigned int previewWidth;
	unsigned int previewHeight;

    if ( MAX_PREVIEW_SURFACE_WIDTH < width )
	{
        previewWidth = MAX_PREVIEW_SURFACE_WIDTH;
    }
	else
	{
        previewWidth = width;
    }

    if ( MAX_PREVIEW_SURFACE_HEIGHT < height )
	{
        previewHeight = MAX_PREVIEW_SURFACE_HEIGHT;
    }
	else
	{
        previewHeight = height;
    }

    client = new SurfaceComposerClient();

    if ( NULL == client.get() )
	{
        printf("Unable to establish connection to Surface Composer \n");

        return -1;
    }

    surfaceControl = client->createSurface(0, previewWidth, previewHeight, pixFormat);
	previewSurface = surfaceControl->getSurface();
	sp<ANativeWindow> kyp = previewSurface.get();


	//status_t error;
	//sp<IGraphicBufferAlloc> asd = kyp->createGraphicBufferAlloc();
	//sp<GraphicBuffer> buff = asd->createGraphicBuffer(width, height, pixFormat, 0, &error);

	//ANativeWindow_acquire(kyp);

    client->openGlobalTransaction();
    surfaceControl->setLayer(0x7fffffff);
    surfaceControl->setPosition(0, 0);
    surfaceControl->setSize(previewWidth, previewHeight);
    surfaceControl->show();
    client->closeGlobalTransaction();

    return 0;
}

int CDisplaySurfaceFlinger::DestroySurface()
{
    if ( NULL != previewSurface.get() )
	{
        previewSurface.clear();
    }

    if ( NULL != surfaceControl.get() )
	{
        surfaceControl->clear();
        surfaceControl.clear();
    }

    if ( NULL != client.get() )
	{
        client->dispose();
        client.clear();
    }

    return 0;
}
