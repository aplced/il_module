#include "inc/ComponentDispatcher/DVPComponent/CDVPComponent.h"

void DVP_KernelGraphCompleted(void *cookie, DVP_KernelGraph_t *graph, DVP_U32 sectionIndex, DVP_U32 numNodesExecuted)
{
    DVP_PRINT(DVP_ZONE_ALWAYS, "TEST: {%p} Graph %p Section [%u] completed %u of %u nodes\n", cookie, graph, sectionIndex, numNodesExecuted, graph->sections[sectionIndex].numNodes);
}

CDVPComponent::CDVPComponent(void)
{
    width = 320;
    height = 240;
    type = DVP_MTYPE_DEFAULT;
    capacity = 100;
    iterations = 1;
    serial = false_e;
    hDVP = 0;
    graph = NULL;
    pNodes = NULL;
    numNodes = 10;
}

void CDVPComponent::DVP_Init_Params(DVP_InitParamsMessage_t* arg)
{
    sem_init(&dvp_sem, 0, 1);
    sem_wait(&dvp_sem);
    width = arg->width;
    height = arg->height;
    type = arg->type;
    capacity = arg->capacity;
    iterations = arg->iterations;
    numNodes = arg->numNodes;
    serial = arg->serial;
    sem_post(&dvp_sem);
   
}

TIMM_OSAL_ERRORTYPE CDVPComponent::Image_Alloc(int nImage, DVP_MemType_e type)
{

   if (DVP_Image_Alloc(hDVP, &images[nImage], type) == DVP_FALSE)
       return TIMM_OSAL_ERR_ALLOC;

    return TIMM_OSAL_ERR_NONE;
}

void CDVPComponent::Image_Init(int nImage, fourcc_t color)
{
   DVP_Image_Init(&images[nImage], width, height, color);
}

TIMM_OSAL_ERRORTYPE CDVPComponent::KernelNode_Alloc()
{
    pNodes = DVP_KernelNode_Alloc(hDVP, numNodes);
    if (!pNodes)
    {
        MMS_IL_PRINT("DVP_KernelNode_Alloc failed!\n");
        return TIMM_OSAL_ERR_ALLOC;
    }
    images = (DVP_Image_t*)malloc((numNodes + 1) * sizeof(DVP_Image_t*));
    return TIMM_OSAL_ERR_NONE;
}

DVP_BOOL CDVPComponent::DVP_Image_Construct( DVP_Image_Params_t *params, DVP_U32 numParams)
{
    DVP_BOOL ret = DVP_TRUE;
    DVP_U32 i = 0;
    for (i = 0; i < numParams; i++)
    {
        DVP_Image_Init(&images[params[i].nImage], width, height, params[i].color);
        images[params[i].nImage].skipCacheOpFlush = DVP_TRUE;
        images[params[i].nImage].skipCacheOpInval = DVP_TRUE;
        if (DVP_Image_Alloc(hDVP, &images[params[i].nImage], params[i].type) == DVP_FALSE)
        {
            MMS_IL_PRINT("DVP_Image_Alloc of image[%d] failed!\n", i);
            ret = DVP_FALSE;
            break;
        }
    }
    return ret;
}

TIMM_OSAL_ERRORTYPE CDVPComponent::KernelGraph_Init()
{
    if (!hDVP)
    {
        // get the handle (will implicitly load .so/.dll and initialize RPC
        hDVP =  DVP_KernelGraph_Init();
    } else
    {
        MMS_IL_PRINT("DVP_KernelGraph_Init is already initied!\n");
        return TIMM_OSAL_ERR_NONE;
    }
    if (!hDVP)
    {
        MMS_IL_PRINT("DVP_KernelGraph_Init failed\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    MMS_IL_PRINT("DVP Handle inited - %d\n", hDVP);
    return TIMM_OSAL_ERR_NONE;
}

void CDVPComponent::Node_Init(DVP_U32 nNode, DVP_U32 nInput, DVP_U32 nOutput, DVP_U32 kernel, DVP_U32 affinity)
{
    DVP_Transform_t *io = NULL;
    pNodes[nNode].header.kernel = kernel;
    io = dvp_knode_to(&pNodes[nNode],DVP_Transform_t);
    DVP_Image_Dup(&io->input, &images[nInput]);
    DVP_Image_Dup(&io->output, &images[nOutput]);
    pNodes[nNode].header.affinity = affinity;
}

void CDVPComponent::Graph_Init(DVP_KernelGraph_t* pgraph)
{
    sem_wait(&dvp_sem);
    graph = pgraph;
    sem_post(&dvp_sem);
}

TIMM_OSAL_ERRORTYPE CDVPComponent::DVP_Init()
{
   DVP_U32 i;
   sem_wait(&dvp_sem);
   MMS_IL_PRINT("Initing DVP ...\n");

    if (hDVP)
    {
        MMS_IL_PRINT("DVP Handle inited - %d\n", hDVP);
        DVP_Image_Params_t params[] =
        {
            { 0, FOURCC_UYVY, (DVP_MemType_e)type},
            { 1, FOURCC_RGBP, DVP_MTYPE_DEFAULT},
            { 2, FOURCC_Y800, DVP_MTYPE_DEFAULT},
            { 3, FOURCC_YU24, DVP_MTYPE_DEFAULT},
            { 4, FOURCC_RGBP, DVP_MTYPE_DEFAULT},
            { 5, FOURCC_Y800, DVP_MTYPE_DEFAULT},
            { 6, FOURCC_IYUV, DVP_MTYPE_DEFAULT},
            { 7, FOURCC_RGBP, DVP_MTYPE_DEFAULT},
            { 8, FOURCC_Y800, DVP_MTYPE_DEFAULT},
            { 9, FOURCC_IYUV, DVP_MTYPE_DEFAULT},
            {10, FOURCC_Y800, DVP_MTYPE_DEFAULT},
        };
        if (DVP_Image_Construct(params, dimof(params)) == DVP_FALSE)
        {
             MMS_IL_PRINT("DVP_Image_Construct failed\n");
         //   goto teardown;//TODO!!!!
            DVP_Deinit();
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        DVP_PRINT(DVP_ZONE_ALWAYS, "DVP_SIMPLE IMAGES:\n");
        for (i = 0; i <= numNodes; i++)
        {
            DVP_PrintImage(DVP_ZONE_ALWAYS, &images[i]);
        }

        // create the nodes (will implicitly map to other cores)
        pNodes = DVP_KernelNode_Alloc(hDVP, numNodes);
        if (pNodes)
        {
            static DVP_KernelGraphSection_t psections[] =
            {
                {&pNodes[0], 3, DVP_PERF_INIT, DVP_CORE_LOAD_INIT, DVP_FALSE},
                {&pNodes[3], 3, DVP_PERF_INIT, DVP_CORE_LOAD_INIT, DVP_FALSE},
                {&pNodes[6], 3, DVP_PERF_INIT, DVP_CORE_LOAD_INIT, DVP_FALSE},
                {&pNodes[9], 1, DVP_PERF_INIT, DVP_CORE_LOAD_INIT, DVP_FALSE},
            };
            static DVP_KernelGraphSection_t ssections[] =
            {
                {&pNodes[0], 10, DVP_PERF_INIT, DVP_CORE_LOAD_INIT, DVP_FALSE},
            };

            DVP_U32            orders[] = {0, 0, 0, 1};
            static DVP_KernelGraph_t pgraph = {
                psections, // make 3 parallel sections of the graph, then a serial section
                dimof(psections),
                orders,
                DVP_PERF_INIT,
            };
            static DVP_KernelGraph_t sgraph = {
                ssections,
                dimof(ssections),
                orders,
                DVP_PERF_INIT,
            };
            //DVP_KernelGraph_t *graph = NULL;
            DVP_Transform_t *io = NULL;
            if (serial == true_e)
                graph = &sgraph;
            else
                graph = &pgraph;

            // A9 processing
            pNodes[0].header.kernel = DVP_KN_UYVY_TO_RGBp;
            io = dvp_knode_to(&pNodes[0],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[1]);
            pNodes[0].header.affinity = DVP_CORE_CPU;

            pNodes[1].header.kernel = DVP_KN_XYXY_TO_Y800;
            io = dvp_knode_to(&pNodes[1],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[2]);
            pNodes[1].header.affinity = DVP_CORE_CPU;

            pNodes[2].header.kernel = DVP_KN_UYVY_TO_YUV444p;
            io = dvp_knode_to(&pNodes[2],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[3]);
            pNodes[2].header.affinity = DVP_CORE_CPU;

            // DSP Processing
            pNodes[3].header.kernel = DVP_KN_UYVY_TO_RGBp;
            io = dvp_knode_to(&pNodes[3],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[4]);
            pNodes[3].header.affinity = DVP_CORE_DSP;

            pNodes[4].header.kernel = DVP_KN_XYXY_TO_Y800;
            io = dvp_knode_to(&pNodes[4],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[5]);
            pNodes[4].header.affinity = DVP_CORE_DSP;

            pNodes[5].header.kernel = DVP_KN_UYVY_TO_YUV420p;
            io = dvp_knode_to(&pNodes[5],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[6]);
            pNodes[5].header.affinity = DVP_CORE_DSP;

            // SIMCOP Processing
            pNodes[6].header.kernel = DVP_KN_UYVY_TO_RGBp;
            io = dvp_knode_to(&pNodes[6],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[7]);
            pNodes[6].header.affinity = DVP_CORE_SIMCOP;

            pNodes[7].header.kernel = DVP_KN_XYXY_TO_Y800;
            io = dvp_knode_to(&pNodes[7],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[8]);
            pNodes[7].header.affinity = DVP_CORE_SIMCOP;

            pNodes[8].header.kernel = DVP_KN_UYVY_TO_YUV420p;
            io = dvp_knode_to(&pNodes[8],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[0]);
            DVP_Image_Dup(&io->output, &images[9]);
            pNodes[8].header.affinity = DVP_CORE_SIMCOP;

            // LAST SECTION
            pNodes[9].header.kernel = DVP_KN_SOBEL_8;
            io = dvp_knode_to(&pNodes[9],DVP_Transform_t);
            DVP_Image_Dup(&io->input, &images[2]);
            DVP_Image_Dup(&io->output, &images[10]);
            pNodes[9].header.affinity = DVP_CORE_GPU;

            for (i = 0; i < DVP_CORE_MAX; i++)
            {
                DVP_U32 cap2, cap1 = DVP_GetCoreCapacity(hDVP, (DVP_Core_e)i);
                DVP_SetCoreCapacity(hDVP, (DVP_Core_e)i, capacity);
                cap2 = DVP_GetCoreCapacity(hDVP, (DVP_Core_e)i);
                DVP_PRINT(DVP_ZONE_ALWAYS, "Core[%u] was set to limit at %u capacity, now at %u\n", i, cap1, cap2);
            }
        }else//if pNodes == 0
        {
                MMS_IL_PRINT("DVP_KernelNode_Alloc failed!\n");
                DVP_Deinit();
                return TIMM_OSAL_ERR_UNKNOWN;
        }//if (pNodes)
    } else //if hDVP == 0
    {
        MMS_IL_PRINT("DVP Handle init failed!\n");
        DVP_Deinit();
        return TIMM_OSAL_ERR_UNKNOWN;
    }//if (hDVP)
    sem_post(&dvp_sem);
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CDVPComponent::DVP_Deinit()
{
    TIMM_OSAL_ERRORTYPE error = 0;
    sem_wait(&dvp_sem);
    MMS_IL_PRINT("Deiniting DVP ...\n");
    DVP_KernelNode_Free(hDVP, pNodes, numNodes);

    // free the image memory
    for (int i = 0; i <= numNodes; i++)
        error += DVP_Image_Free(hDVP, &images[i]);
    sem_post(&dvp_sem);
     return error;
}

TIMM_OSAL_ERRORTYPE CDVPComponent::KernelGraph_Deinit()
{
    TIMM_OSAL_ERRORTYPE error = 0;
    sem_wait(&dvp_sem);
    DVP_KernelGraph_Deinit(hDVP);
    hDVP = 0;
    sem_post(&dvp_sem);
    return error;
}
TIMM_OSAL_ERRORTYPE CDVPComponent::Image_Free(DVP_U32 nImage)
{
    TIMM_OSAL_ERRORTYPE error = 0;
    sem_wait(&dvp_sem);
    error = DVP_Image_Free(hDVP, &images[nImage]);
    sem_post(&dvp_sem);
    return error;
}

TIMM_OSAL_ERRORTYPE CDVPComponent::KernelNode_Free()
{
    TIMM_OSAL_ERRORTYPE error = 0;

    sem_wait(&dvp_sem);
    DVP_KernelNode_Free(hDVP, pNodes, numNodes);
    if (images)
        free(images);
    sem_post(&dvp_sem);

    return error;
}


DVP_U32 CDVPComponent::Get_Core_Capacity(DVP_Core_e i)
{
    sem_wait(&dvp_sem);
    DVP_U32 cap = DVP_GetCoreCapacity(hDVP, i);
    sem_post(&dvp_sem);
   return cap;
}

void CDVPComponent::Set_Core_Capacity(DVP_Core_e i, DVP_U32 capacity)
{
    sem_wait(&dvp_sem);
    DVP_SetCoreCapacity(hDVP, i, capacity);
    sem_post(&dvp_sem);
}

void CDVPComponent::PrintPerformanceGraph()
{
   sem_wait(&dvp_sem);
   DVP_PrintPerformanceGraph(hDVP, graph);
   sem_post(&dvp_sem);
}

void CDVPComponent::DVP_Start()
{
    pthread_join(DVP_process_thread, NULL);

    MMS_IL_PRINT("Starting DVP_process_thread!\n");
    if (pthread_create(&DVP_process_thread, NULL, DVP_process_thread_func, (void*)this))
    {
        MMS_IL_PRINT("Error Starting DVP_process_thread_func\n");
    }
}
CDVPComponent::~CDVPComponent()
{
}

void* DVP_process_thread_func(void* arg)
{
    CDVPComponent* DVP_arg  = (CDVPComponent*) arg;
    DVP_U32 numSections = 0;
    DVP_U32 iterations =  DVP_arg->iterations;
    MMS_IL_PRINT("DVP_process_thread started!\n");
    MMS_IL_PRINT("hDVP %d, graph %p \n", DVP_arg->hDVP, DVP_arg->graph);
    sem_wait(&DVP_arg->dvp_sem);
    do {
        MMS_IL_PRINT("Iteration %u\n", iterations);
        // DVP_PRINT(DVP_ZONE_ALWAYS, "Iteration %u\n", iterations);
        // perform the graph processing
        numSections = DVP_KernelGraph_Process(DVP_arg->hDVP, DVP_arg->graph, NULL, DVP_KernelGraphCompleted);
        MMS_IL_PRINT("%u Sections Completed!\n", numSections);
        //DVP_PRINT(DVP_ZONE_ALWAYS, "%u Sections Completed!\n", numSections);
     } while (iterations-- && numSections > 0);

    // show us the results
  //  DVP_PrintPerformanceGraph(DVP_arg->hDVP, DVP_arg->graph);
    MMS_IL_PRINT("DVP_process_thread Finished!\n");
    sem_post(&DVP_arg->dvp_sem);
    return NULL;
}