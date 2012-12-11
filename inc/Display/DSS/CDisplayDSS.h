#ifndef __CDisplayDSS__H__
#define __CDisplayDSS__H__

#include "inc/Display/CDisplay.h"
#include <dsscomp.h>
#include <OMX_IVCommon.h>

#define DSSCOMP_DEVICE "/dev/dsscomp"
#define REQUIRED_OVERLAYS 1

//	OMAP_DSS_GFX	= 0
//	OMAP_DSS_VIDEO1	= 1
//	OMAP_DSS_VIDEO2	= 2
//	OMAP_DSS_VIDEO3	= 3
//	OMAP_DSS_WB		= 4


#define MAX_OVLS_PER_COMPOSITION 3
#define DSSCOMP_WAIT_TIMEOUT 50000 //in microseconds

static const struct omap_dss_cconv_coefs CTBL_BT601_5 =
{
	298,  409,    0,
	298, -208, -100,
	298,    0,  517,
	0
};

class CDisplayDSS : public CDisplay
{
public:
	CDisplayDSS();
    TIMM_OSAL_ERRORTYPE Init(DisplayConfig* cfg);
    void Deinit();
    void Config(DisplayConfig* cfg);
    void GetConfig(DisplayConfig* cfg);
    int PerFrameConfig(PerFrameConfiguration cfg);
    void* DisplayFrame(void* addrY, void* addrUV, int cropX, int cropY, int cropWidth, int cropHeight);

private:
	int DSSfd;
	unsigned int mActivePipe_idx;
	struct dsscomp_setup_mgr_data* DSSdata;
	unsigned int DispayW;
	unsigned int DispayH;
	DisplayConfig DSScfg;

	TIMM_OSAL_ERRORTYPE DSS_Init();
	void DSS_DeInit();
	TIMM_OSAL_ERRORTYPE DSS_ConfigFrameData(OMX_COLOR_FORMATTYPE pix_fmt, unsigned int w, unsigned int h, unsigned int stride);
	void* DSS_DisplayFrameBuffer(void* addr, void* uv_addr, int crop_x, int crop_y, int crop_w, int crop_h, int ovlId);
	void DSS_GetConfig(DisplayConfig* cfg);
	void DSS_SetConfig(DisplayConfig* cfg);
    void DSS_InitSetConfig(DisplayConfig* cfg);
	enum omap_color_mode omx_to_dss_colorfmt(OMX_COLOR_FORMATTYPE pix_fmt);
	int open_dss(const char* dss_dev);
	int wait_dss_event(int data_fd, unsigned int timeout_us, enum dsscomp_wait_phase event);
	int query_display(int dss_fd, unsigned int disp_num, struct dsscomp_display_info* display_info);
	int set_dss_mgr_data(int dss_fd, enum dsscomp_setup_mode mode, struct dsscomp_setup_mgr_data* mgr_data, int get_sync);
	int set_dss_dispc_data(int dss_fd, enum dsscomp_setup_mode mode, struct dss2_mgr_info* mgr_info, struct dss2_ovl_info* ovl_info, int get_sync);
	TIMM_OSAL_ERRORTYPE init_dss_composition(int dss_fd, unsigned int display_ix, unsigned int requestedOvls, struct dsscomp_setup_mgr_data** mgr_data);
	void auto_adjust_window(struct dss2_ovl_info* ovl);
	void force_adjust_window(struct dss2_ovl_info* ovl);
	int check_dss_overlay(int dss_fd, enum dsscomp_setup_mode mode, struct dsscomp_setup_mgr_data* mgr_data, int ovlId);
};

#endif