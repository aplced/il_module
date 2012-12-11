#ifndef __CDVPComponent__H__
#define __CDVPComponent__H__

#include <sosal/sosal.h>


#include "inc/lib_comm.h"
#include <sys/time.h>
#include <sys/resource.h>

void* DVP_process_thread_func(void* arg);
void DVP_KernelGraphCompleted(void *cookie, DVP_KernelGraph_t *graph, DVP_U32 sectionIndex, DVP_U32 numNodesExecuted);

class CDVPComponent
{
public:
    CDVPComponent(void);
     ~CDVPComponent(void);
    DVP_Handle DVPCompHdl() {return hDVP;}
	//COMXComponentDispatcher *dispatcher;
    TIMM_OSAL_ERRORTYPE Image_Alloc(int nImage, DVP_MemType_e type);
    TIMM_OSAL_ERRORTYPE KernelNode_Alloc();
    TIMM_OSAL_ERRORTYPE KernelGraph_Init();
    TIMM_OSAL_ERRORTYPE DVP_Init();
    TIMM_OSAL_ERRORTYPE DVP_Deinit();
    TIMM_OSAL_ERRORTYPE KernelGraph_Deinit();
    TIMM_OSAL_ERRORTYPE Image_Free(DVP_U32 nImage);
    TIMM_OSAL_ERRORTYPE KernelNode_Free();
    DVP_U32 Get_Core_Capacity(DVP_Core_e i);
    DVP_BOOL DVP_Image_Construct( DVP_Image_Params_t *params, DVP_U32 numParams);
    void Node_Init(DVP_U32 nNode, DVP_U32 nInput, DVP_U32 nOutput, DVP_U32 kernel, DVP_U32 affinity);
    void Image_Init(int nImage, fourcc_t color);
    void Graph_Init(DVP_KernelGraph_t* pgraph);
    void Set_Core_Capacity(DVP_Core_e i, DVP_U32 capacity);
    void DVP_Init_Params(DVP_InitParamsMessage_t* arg);
    void PrintPerformanceGraph();
    void DVP_Start();
    TIMM_OSAL_U8 nComponentId;
private:
    sem_t dvp_sem;
    DVP_U32 numNodes;
    uint32_t width;
    uint32_t height;
    uint32_t type;
    uint32_t capacity;
    uint32_t iterations;
    bool_e   serial;
    friend void DVP_KernelGraphCompleted(void *cookie, DVP_KernelGraph_t *graph, DVP_U32 sectionIndex, DVP_U32 numNodesExecuted);
    friend  void* DVP_process_thread_func(void* arg);
    DVP_Handle hDVP;
    DVP_KernelGraph_t *graph;
    DVP_KernelNode_t *pNodes;
    pthread_t DVP_process_thread;
    DVP_Image_t* images;
    uint32_t numOpts;// = dimof(opts);
   
};

#endif  