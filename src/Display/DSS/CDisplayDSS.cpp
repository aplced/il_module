extern "C" {
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
}
#include "inc/Display/DSS/CDisplayDSS.h"
#include "inc/lib_comm.h"
#define USED_OVERLAY 2
/*
extern TIMM_OSAL_U32 disp_Forced;
extern TIMM_OSAL_U32 disp_overlay;
extern TIMM_OSAL_U32 disp_Zorder;
extern TIMM_OSAL_U32 disp_start_x;
extern TIMM_OSAL_U32 disp_start_y;
extern TIMM_OSAL_U32 disp_w;
extern TIMM_OSAL_U32 disp_h;
*/
//#define MMS_IL_PRINT(ARGS...)
CDisplayDSS::CDisplayDSS()
{
    DSSfd = 0;
    DSSdata = NULL;
    DispayW = 0;
    DispayH = 0;
    DSScfg.disp_Forced  = 0;
    DSScfg.disp_overlay = USED_OVERLAY;
    DSScfg.disp_Zorder  = 0;
    DSScfg.disp_start_x = 0;
    DSScfg.disp_start_y = 0;
    DSScfg.disp_w = 1000;  // set some big number. It will be clipped up to display capabilities
    DSScfg.disp_h = 1000;  // set some big number. It will be clipped up to display capabilities
    DSScfg.autoAdjustWindow = 1;
    DSScfg.rotation = 0;
    DSScfg.mirror = 0;
    DSScfg.globalAlpha = 128;
}

TIMM_OSAL_ERRORTYPE CDisplayDSS::Init(DisplayConfig* cfg)
{

    if (cfg)
        DSS_InitSetConfig(cfg);
    return DSS_Init();
}

void CDisplayDSS::Deinit()
{
    DSS_DeInit();
}

void CDisplayDSS::Config(DisplayConfig* cfg)
{
    DSS_SetConfig(cfg);
}

void CDisplayDSS::GetConfig(DisplayConfig* cfg)
{
    DSS_GetConfig(cfg);
}

int CDisplayDSS::PerFrameConfig(PerFrameConfiguration cfg)
{
    return DSS_ConfigFrameData((OMX_COLOR_FORMATTYPE)cfg.format, cfg.width, cfg.height, cfg.stride);
}

void* CDisplayDSS::DisplayFrame(void* addrY, void* addrUV, int cropX, int cropY, int cropWidth, int cropHeight)
{
    return DSS_DisplayFrameBuffer(addrY, addrUV, cropX, cropY, cropWidth, cropHeight, mActivePipe_idx);
}

enum omap_color_mode CDisplayDSS::omx_to_dss_colorfmt(OMX_COLOR_FORMATTYPE pix_fmt)
{
    switch (pix_fmt) {
        case OMX_COLOR_FormatCbYCrY:
            return OMAP_DSS_COLOR_UYVY;

        case OMX_COLOR_Format16bitRGB565:
            return OMAP_DSS_COLOR_RGB16;

        case OMX_COLOR_Format32bitARGB8888:
            return OMAP_DSS_COLOR_ARGB32;

        case OMX_COLOR_FormatYUV420SemiPlanar:
        case OMX_COLOR_FormatYUV420PackedSemiPlanar:
            return OMAP_DSS_COLOR_NV12;

        default:
            return (omap_color_mode)0;
    }
}

int CDisplayDSS::open_dss(const char* dss_dev)
{
    int dss_fd = 0;

    /*DSScfg.rotation = 0;
    DSScfg.mirror = 0;
    DSScfg.globalAlpha = 128;
    DSScfg.autoAdjustWindow = 1;
    DSScfg.window.left = 0;
    DSScfg.window.right = 0;
    DSScfg.window.width = 0;
    DSScfg.window.height = 0;*/
    //Open Display
    dss_fd = open(dss_dev, O_RDWR);
    if (dss_fd <= 0)
    {
        return 0;
    }

    return dss_fd;
}

int CDisplayDSS::wait_dss_event(int data_fd, unsigned int timeout_us, enum dsscomp_wait_phase event)
{
    struct dsscomp_wait_data wait_data;

    wait_data.timeout_us = timeout_us;
    wait_data.phase = event;
    return ioctl(data_fd, DSSCIOC_WAIT, &wait_data);
}

int CDisplayDSS::query_display(int dss_fd, unsigned int disp_num, struct dsscomp_display_info* display_info)
{
    display_info->ix = disp_num;
    return ioctl(dss_fd, DSSCIOC_QUERY_DISPLAY, display_info);
}

int CDisplayDSS::set_dss_mgr_data(int dss_fd, enum dsscomp_setup_mode mode, struct dsscomp_setup_mgr_data* mgr_data, int get_sync)
{
    mgr_data->get_sync_obj = get_sync;
    mgr_data->mode = mode;
    //returns data sync fd on success, < 0 on failure
    return ioctl(dss_fd, DSSCIOC_SETUP_MGR, mgr_data);
}

int CDisplayDSS::check_dss_overlay(int dss_fd, enum dsscomp_setup_mode mode, struct dsscomp_setup_mgr_data* mgr_data, int ovlId)
{
    struct dsscomp_check_ovl_data chk;
    chk.mode = mode;
    memcpy(&(chk.mgr), mgr_data, sizeof(struct dsscomp_setup_mgr_data));
    memcpy(&(chk.ovl), &(mgr_data->ovls[ovlId]), sizeof(struct dss2_ovl_info));

    return ioctl(dss_fd, DSSCIOC_CHECK_OVL, &chk);
}

int CDisplayDSS::set_dss_dispc_data(int dss_fd, enum dsscomp_setup_mode mode, struct dss2_mgr_info* mgr_info, struct dss2_ovl_info* ovl_info, int get_sync)
{
    struct dsscomp_setup_dispc_data dispc_data;

    dispc_data.mode = mode;

    dispc_data.num_mgrs = 1;
    dispc_data.num_ovls = 5;
    dispc_data.get_sync_obj = get_sync;

    dispc_data.mgrs[0] = *mgr_info;
    memcpy(dispc_data.ovls, ovl_info, sizeof(struct dss2_ovl_info)*5);

    //returns data sync fd on success, < 0 on failure
    return ioctl(dss_fd, DSSCIOC_SETUP_DISPC, &dispc_data);
}

TIMM_OSAL_ERRORTYPE CDisplayDSS::init_dss_composition(int dss_fd, unsigned int display_ix, unsigned int requestedOvls, struct dsscomp_setup_mgr_data** mgr_data)
{
    struct dsscomp_display_info disp_info;
    struct dss2_ovl_info* ovlInfo;
    unsigned int disp_ovls = 0;

    if (requestedOvls < 1 || requestedOvls > MAX_OVLS_PER_COMPOSITION)
    {
        MMS_IL_PRINT("Illegal number of overlays per composition (%d)\n", requestedOvls);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    memset((void *) &disp_info, 0, sizeof(disp_info));

    if (query_display(dss_fd, display_ix, &disp_info))
    {
        MMS_IL_PRINT("Display ID %d query failed\n", display_ix);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    //if(disp_info.state != OMAP_DSS_DISPLAY_ACTIVE)
    //{
    //    MMS_IL_PRINT("Display ID %d is in non active state: %d\n", display_ix, disp_info.state);
    //}

    MMS_IL_PRINT("Owned overlays 0x%08X\n", disp_info.overlays_owned);
    MMS_IL_PRINT("Available overlays 0x%08X\n", disp_info.overlays_available);

    // Count overlays on the output display
    for (int i = disp_info.overlays_owned; i; i >>= 1)
    {
        if (i & 1)
        {
           disp_ovls++;
        }
    }

    // TODO: In the below case claim the needed resources
    if (disp_ovls < requestedOvls)
    {
        MMS_IL_PRINT("Ovls needed: %d, Ovls attached to display: %d\n", requestedOvls, disp_ovls);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    *mgr_data = (struct dsscomp_setup_mgr_data*) malloc(sizeof(struct dsscomp_setup_mgr_data) + sizeof(struct dss2_ovl_info)*disp_ovls);
    ovlInfo = (struct dss2_ovl_info*)((*mgr_data) + sizeof(struct dsscomp_setup_mgr_data));

    if (*mgr_data == NULL)
    {
        MMS_IL_PRINT("Manager data memory allocation failed\n");
        return TIMM_OSAL_ERR_ALLOC;
    }

    memset((void*)*mgr_data, 0, sizeof(struct dsscomp_setup_mgr_data) + sizeof(struct dss2_ovl_info)*disp_ovls);

    (*mgr_data)->mgr = disp_info.mgr;
    (*mgr_data)->mgr.ix = disp_info.channel;
    (*mgr_data)->mgr.trans_enabled = 0;
    (*mgr_data)->mgr.interlaced = 0;
    (*mgr_data)->mgr.alpha_blending = 1;
    (*mgr_data)->mgr.cpr_enabled = 0;
    (*mgr_data)->mgr.swap_rb = 0;

    (*mgr_data)->win.x = 0;
    (*mgr_data)->win.y = 0;
    (*mgr_data)->win.w = disp_info.timings.x_res;
    (*mgr_data)->win.h = disp_info.timings.y_res;

    MMS_IL_PRINT("Display W = %d H = %d\n", disp_info.timings.x_res, disp_info.timings.y_res);
    MMS_IL_PRINT("Using (*mgr_data)->mgr.ix: %d\n", (*mgr_data)->mgr.ix);
    //ovlInfo->addressing = OMAP_DSS_BUFADDR_DIRECT;

    /* Update user window config data with the display resolution */
    /*DSScfg.window.left = 0;
    DSScfg.window.right = 0;
    DSScfg.window.width  = disp_info.timings.x_res;
    DSScfg.window.height = disp_info.timings.y_res;

    // Store display resolution for automatic window adjustment calc */
    /*DispayW = DSScfg.window.width;
    DispayH = DSScfg.window.height;
    */
    DispayW = disp_info.timings.x_res;
    DispayH = disp_info.timings.y_res;
// clip display size up to physical display dimentions
    if (DSScfg.disp_w > DispayW)
        DSScfg.disp_w = DispayW;
    if (DSScfg.disp_h > DispayH)
        DSScfg.disp_h = DispayH;

    int ownedOvls = disp_info.overlays_owned;
    unsigned int ix = 0;
    unsigned int ix_saved = 0;
    int zOrder = 3;
    //int test;
    ownedOvls &= (0x00000001|(1<<DSScfg.disp_overlay)); // Select GFX (will be disabled from following loop) and selected overlay

    while((ownedOvls != 0) && (ix_saved < disp_ovls))
    {
        if (ownedOvls & 1)
        {
            struct dss2_ovl_info* ovl;

            if(ix != DSScfg.disp_overlay)
            {
                MMS_IL_PRINT("Stopped overlay %d\n", ix);
                ovl = &(*mgr_data)->ovls[ix_saved];

                ovl->cfg.ix = ix;
                ovl->cfg.mgr_ix = disp_info.channel;

                ovl->cfg.zonly = 0;
                ovl->cfg.zorder = zOrder--;
            }
            else
            {

                ovl = &(*mgr_data)->ovls[ix_saved];
                ovl->cfg.ix = ix;
                ovl->cfg.mgr_ix = disp_info.channel;

                mActivePipe_idx = ix_saved; // Save index to Active (displayed) pipeline
                ovl->cfg.zonly  = 0;
                ovl->cfg.zorder = DSScfg.disp_Zorder;

                ovl->cfg.cconv = CTBL_BT601_5;
                ovl->cfg.ilace = OMAP_DSS_ILACE_NONE;
                //User config section. Setting the default values
                ovl->cfg.global_alpha = DSScfg.globalAlpha;
                ovl->cfg.rotation = DSScfg.rotation;
                ovl->cfg.mirror = DSScfg.mirror;
                ovl->cfg.win.w = DSScfg.disp_w;
                ovl->cfg.win.h = DSScfg.disp_h;
                MMS_IL_PRINT("Using overlay %d\n", ix);
            }
            ix_saved++;
            //test = check_dss_overlay(DSSfd, DSSCOMP_SETUP_MODE_APPLY, DSSdata, ix);
            //MMS_IL_PRINT("DSSCIOC_CHECK_OVL %d returnede 0x%08X\n", ix, test);
        } else {
            MMS_IL_PRINT("Skipped overlay %d\n", ix);
        }

        ownedOvls >>= 1;
        ix++;
    }

    (*mgr_data)->num_ovls = ix_saved;

    //test = set_dss_mgr_data(DSSfd, DSSCOMP_SETUP_MODE_APPLY, DSSdata, 0);
    //MMS_IL_PRINT("Setting up DSS manager data status %d (0x%08X)\n", test, test);

    return TIMM_OSAL_ERR_NONE;
}

void CDisplayDSS::force_adjust_window(struct dss2_ovl_info* ovl)
{
unsigned int w = ovl->cfg.crop.w;
unsigned int h = ovl->cfg.crop.h;

    if (DSScfg.disp_w * h > DSScfg.disp_h * w)
    {
        if (h > DSScfg.disp_h)
        {
            ovl->cfg.win.h = DSScfg.disp_h;
            ovl->cfg.win.w = DSScfg.disp_h * w / h;
        }
        else
        {
            ovl->cfg.win.h = h;
            ovl->cfg.win.w = w;
        }
    }
    else
    {
        if (w > DSScfg.disp_w)
        {
            ovl->cfg.win.w = DSScfg.disp_w;
            ovl->cfg.win.h = DSScfg.disp_w * h / w;
        }
        else
        {
            ovl->cfg.win.w = w;
            ovl->cfg.win.h = h;
        }
    }

    ovl->cfg.win.x = (DSScfg.disp_w - ovl->cfg.win.w) / 2;
    ovl->cfg.win.y = (DSScfg.disp_h - ovl->cfg.win.h) / 2;

    ovl->cfg.win.x += DSScfg.disp_start_x;
    ovl->cfg.win.y += DSScfg.disp_start_y;
}

void CDisplayDSS::auto_adjust_window(struct dss2_ovl_info* ovl)
{
    unsigned int w = ovl->cfg.crop.w;
    unsigned int h = ovl->cfg.crop.h;

    if (DSScfg.disp_Forced)
    {
        force_adjust_window(ovl);
        return;
    }

    if (DispayW * h > DispayH * w)
    {
        if (h > DispayH)
        {
            ovl->cfg.win.h = DispayH;
            ovl->cfg.win.w = DispayH * w / h;
        }
        else
        {
            ovl->cfg.win.h = h;
            ovl->cfg.win.w = w;
        }
    }
    else
    {
        if (w > DispayW)
        {
            ovl->cfg.win.w = DispayW;
            ovl->cfg.win.h = DispayW * h / w;
        }
        else
        {
            ovl->cfg.win.w = w;
            ovl->cfg.win.h = h;
        }
    }

    ovl->cfg.win.x = (DispayW - ovl->cfg.win.w) / 2;
    ovl->cfg.win.y = (DispayH - ovl->cfg.win.h) / 2;

 /*
    ovl->cfg.win.w /=2;
    ovl->cfg.win.h /=2;
    ovl->cfg.win.x = 256;//(DispayW - ovl->cfg.win.w) / 2;
    {
        static int i  = 0;
        ovl->cfg.win.x = i++;//(DispayW - ovl->cfg.win.w) / 2;
        if (i>512) i = 0;
        MMS_IL_PRINT("-->>>ovl->cfg.win.x = %d\n", ovl->cfg.win.x);
    }
    ovl->cfg.win.y = 0;//(DispayH - ovl->cfg.win.h) / 2;
*/

}

TIMM_OSAL_ERRORTYPE CDisplayDSS::DSS_Init()
{
    if (DSSfd > 0)
    {
        MMS_IL_PRINT("Display device '%s' already opened\n", DSSCOMP_DEVICE);
        return TIMM_OSAL_ERR_NONE;
    }

    DSSfd = open_dss(DSSCOMP_DEVICE);
    if (DSSfd <= 0)
    {
        MMS_IL_PRINT("Failed to open '%s'\n", DSSCOMP_DEVICE);
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    MMS_IL_PRINT("Opened display device '%s'\n", DSSCOMP_DEVICE);

    if(DSSdata != NULL)
    {
        MMS_IL_PRINT("Releasing previous DSSdata\n");
        free(DSSdata);
    }
    if (TIMM_OSAL_ERR_NONE != init_dss_composition(DSSfd, OMAP_DSS_CHANNEL_LCD, REQUIRED_OVERLAYS, &DSSdata)) {
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    return TIMM_OSAL_ERR_NONE;
}

void CDisplayDSS::DSS_DeInit()
{
    if (DSSfd > 0)
    {
        if (DSSdata != NULL)
        {
            for(int i = 0; i < REQUIRED_OVERLAYS; i++)
            {
                if (DSSdata->ovls[i].cfg.enabled != 0)
                {
                    DSS_DisplayFrameBuffer(NULL, NULL, 0, 0, 0, 0, i);
                }
            }
            free(DSSdata);
            DSSdata = NULL;
        }
        close(DSSfd);
        DSSfd = 0;

        MMS_IL_PRINT("Closed display device '%s'\n", DSSCOMP_DEVICE);
    }
}

TIMM_OSAL_ERRORTYPE CDisplayDSS::DSS_ConfigFrameData(OMX_COLOR_FORMATTYPE pix_fmt, unsigned int w, unsigned int h, unsigned int stride)
{
    if (DSSfd <= 0)
    {
        MMS_IL_PRINT("Display device not init\n");
        return TIMM_OSAL_ERR_NOT_READY;
    }

    if (DSSdata == NULL)
    {
        MMS_IL_PRINT("DSS data not init\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    int ovlId = mActivePipe_idx;

    DSSdata->ovls[ovlId].cfg.color_mode = omx_to_dss_colorfmt(pix_fmt);
    if (DSSdata->ovls[ovlId].cfg.color_mode == 0)
    {
        MMS_IL_PRINT("Color format %d not supported\n", pix_fmt);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    DSSdata->ovls[ovlId].cfg.width = w;
    DSSdata->ovls[ovlId].cfg.height = h;
    DSSdata->ovls[ovlId].cfg.stride = stride;
    DSScfg.autoAdjustWindow = 1;

    MMS_IL_PRINT("DSS frame configuration: %d x %d, stride %d, color format %d\n", w, h, stride, pix_fmt);
    return TIMM_OSAL_ERR_NONE;
}

void* CDisplayDSS::DSS_DisplayFrameBuffer(void* addr, void* uv_addr, int crop_x, int crop_y, int crop_w, int crop_h, int ovlId)
{
    static int data_fd = -1;
    void* current_addr;
//    int ovlId = mActivePipe_idx;
   // MMS_IL_PRINT("DSS_DisplayFrameBuffer\n");

    if (DSSfd <= 0)
    {
        MMS_IL_PRINT("Display device not init\n");
        return addr;
    }

    if (DSSdata == NULL)
    {
        MMS_IL_PRINT("DSS data not init\n");
        return addr;
    }

    current_addr = DSSdata->ovls[ovlId].address;
    if (addr == NULL)
    {
        MMS_IL_PRINT("Disabling overlays\n");
        DSSdata->ovls[ovlId].cfg.enabled = 0;
        DSSdata->ovls[ovlId].cfg.zonly = 1;
        DSSdata->ovls[ovlId].address = NULL;
        if (data_fd >= 0)
        {
            close(data_fd);
            data_fd = -1;
        }
        set_dss_mgr_data(DSSfd, DSSCOMP_SETUP_DISPLAY, DSSdata, 0);
    }
    else//IF (addr != NULL)
    {
        //MMS_IL_PRINT("addr != NULL\n");
        if (crop_w == 0 || crop_h == 0)
        {
            MMS_IL_PRINT("Zero crop\n");
            return addr;
        }

        // If previous data is available check if it is displayed
        if (data_fd >= 0)
        {
            if (wait_dss_event(data_fd, DSSCOMP_WAIT_TIMEOUT, DSSCOMP_WAIT_DISPLAYED) < 0)
            {
                MMS_IL_PRINT("Previous frame pending to be displayed\n");
                return addr;
            }
            close(data_fd);
        }
        DSSdata->ovls[ovlId].address = addr;
        if (DSSdata->ovls[ovlId].cfg.color_mode == OMAP_DSS_COLOR_NV12)
        {
            DSSdata->ovls[ovlId].uv_address = uv_addr;
        }
        else
        {
            DSSdata->ovls[ovlId].uv_address = NULL;
        }
        MMS_IL_PRINT("DSSdata->ovls[ovlId].cfg.color_mode - %d\n", DSSdata->ovls[ovlId].cfg.color_mode);

        DSSdata->ovls[ovlId].cfg.crop.x = crop_x;
        DSSdata->ovls[ovlId].cfg.crop.y = crop_y;
        DSSdata->ovls[ovlId].cfg.crop.w = crop_w;
        DSSdata->ovls[ovlId].cfg.crop.h = crop_h;
        DSSdata->ovls[ovlId].cfg.enabled = 1;
        DSSdata->ovls[ovlId].cfg.zonly = 0;
//        DSScfg.autoAdjustWindow = 1;
        if (DSScfg.autoAdjustWindow)
        {
            auto_adjust_window(&DSSdata->ovls[ovlId]);
            DSScfg.autoAdjustWindow = 0;
        }

        data_fd = check_dss_overlay(DSSfd, DSSCOMP_SETUP_DISPLAY, DSSdata, ovlId);
        MMS_IL_PRINT("DSSCIOC_CHECK_OVL returned 0x%08X\n", data_fd);

        data_fd = set_dss_mgr_data(DSSfd, DSSCOMP_SETUP_DISPLAY, DSSdata, 1);
       // MMS_IL_PRINT("DSSCIOC_SETUP_MGR returned 0x%08X\n", data_fd);

        if (data_fd < 0)
        {
            MMS_IL_PRINT("Setting up DSS manager data failed with status %d (0x%08X)\n", data_fd, data_fd);
            DSSdata->ovls[ovlId].address = current_addr;
            return addr;
        }
    }//IF (addr == NULL)

    return current_addr;
}

void CDisplayDSS::DSS_GetConfig(DisplayConfig* cfg)
{
    *cfg = DSScfg;
}


void CDisplayDSS::DSS_InitSetConfig(DisplayConfig* cfg)
{
    DSScfg = *cfg;
}

void CDisplayDSS::DSS_SetConfig(DisplayConfig* cfg)
{
    DSScfg = *cfg;

    DSSdata->ovls[mActivePipe_idx].cfg.global_alpha = DSScfg.globalAlpha;
    DSSdata->ovls[mActivePipe_idx].cfg.rotation = DSScfg.rotation;
    DSSdata->ovls[mActivePipe_idx].cfg.mirror = DSScfg.mirror;
    if (!DSScfg.autoAdjustWindow)
    {
        DSSdata->ovls[mActivePipe_idx].cfg.win.x = DSScfg.disp_start_x;
        DSSdata->ovls[mActivePipe_idx].cfg.win.y = DSScfg.disp_start_y;
        DSSdata->ovls[mActivePipe_idx].cfg.win.w = DSScfg.disp_w;
        DSSdata->ovls[mActivePipe_idx].cfg.win.h = DSScfg.disp_h;
    }
}