#include <stdio.h>
#include <unistd.h>
#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/COMXComponentDispatcher.h"
char tcp_port[5];


/*#define USED_OVERLAY 2
TIMM_OSAL_U32 disp_Forced  = 0;
TIMM_OSAL_U32 disp_overlay = USED_OVERLAY;
TIMM_OSAL_U32 disp_Zorder  = 0;
TIMM_OSAL_U32 disp_start_x = 0;
TIMM_OSAL_U32 disp_start_y = 0;
TIMM_OSAL_U32 disp_w = 1000;  // set some big number. It will be clipped up to display capabilities
TIMM_OSAL_U32 disp_h = 1000;  // set some big number. It will be clipped up to display capabilities
*/

int main(int argc, char *argv[])
{
    int ret = 0;

    printf("\n\n***************************\n");
    printf("MMS OMAP Server IL version %08X\n", VER);
    printf("***************************\n");

	// Initizalize OMX component
    ret = OMX_Init();

	if (ret)
	{
        printf("OMX_Init failed: %d\n",ret);
    }


    if (argc > 1) strcpy(tcp_port, argv[1]);
    else  strcpy(tcp_port, TCPIP_PORT);

/*    { // Init default values
        strcpy(tcp_port, TCPIP_PORT);
        disp_start_x = 0;
        disp_start_y = 0;
        disp_w = 10000;  // set some big number. It will be clipped up to display capabilities
        disp_h = 10000;  // set some big number. It will be clipped up to display capabilities
        disp_overlay = USED_OVERLAY;
    }
    {
    int opt;

    while ((opt = getopt(argc, argv, "x:y:w:h:p:o:z:")) != -1) {
            switch (opt) {
            case 'x':
                disp_start_x = atoi(optarg);
                disp_Forced = 1;
                break;

            case 'y':
                disp_start_y = atoi(optarg);

                disp_Forced = 1;
                break;

            case 'w':
                disp_w = atoi(optarg);
                disp_Forced = 1;
                break;

            case 'h':
                disp_h = atoi(optarg);
                disp_Forced = 1;
                break;

            case 'o':
                disp_overlay = atoi(optarg);
                break;

            case 'z':
                disp_Zorder = atoi(optarg);
                break;

            case 'p':
                strcpy(tcp_port, optarg);
                break;

            default: /* '?'
                fprintf(stderr, "Usage: %s [-p tcp_port] [-x display start X] x [-y display start Y] [-w display width] [-h display height] [-o overlay (1-3)] [-o Zorder (0-2)]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }

    }
    */

    printf("TCP port is : %d\n",atoi(tcp_port));
    //printf("Display : o = %d z = %d x = %d y = %d w = %d h = %d\n", disp_overlay, disp_Zorder, disp_start_x, disp_start_y, disp_w, disp_h);

    while (1)
    {
	    //create object of type component dispatcher
        COMXComponentDispatcher* omxDispatcher = new COMXComponentDispatcher();
        omxDispatcher->Start();
        delete(omxDispatcher);
        printf("OMAP Server IL restarting connection...\n");
    }

    return 0;
}
