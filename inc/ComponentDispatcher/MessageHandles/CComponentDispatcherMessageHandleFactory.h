#ifndef __CComponentDispatcherMessageHandleFactory__H__
#define __CComponentDispatcherMessageHandleFactory__H__

#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherCommandHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSConfigHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSInitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherEmptyThisBufferHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherFillThisBufferHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetConfigHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetHandleHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetILClientVersionHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetOMXVersionHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetParamHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetPortInfoHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalFileCopyHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocTypeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemCopyHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemDumpHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemFreeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemReadHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAllocateBufferHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAutoBuffCycleHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortConfigAllocTypeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortLinkHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortSetUpServiceHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortUseMarshalledMemHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseHandleHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetConfigHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetParamHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetPortBufferCountHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherStreamerAttachHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherConfigPortServicesHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDisconnectTCPHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetI2CHandleHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CReadHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CWriteHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseI2CHandleHandle.h"
#ifdef DVP_ENABLE
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPAllocKernelNodeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPDeinitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPGetCoreCapacityHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageAllocHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageConstructHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageFreeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageInitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitNodeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitParamsHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelGraphDeinitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelGraphInitHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelNodeFreeHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPPrintPerformanceGraphHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPSetCoreCapacityHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPStartKGraphProcessHandle.h"
#endif

class CComponentDispatcherMessageHandleFactory
{
public:
	static CComponentDispatcherMessageHandle* CreateMessageHandle(COMXComponentDispatcher* hDispatcher, unsigned int hId)
	{
		switch(hId)
		{
			case OMX_COMMAND_MESSAGE:
                //executes message commands
                return new CComponentDispatcherCommandHandle(hDispatcher, OMX_COMMAND_MESSAGE);
            case SYS_TCP_DISCONNECT:
                //Diconnect TCP channel and stop all IL processes
                return new CComponentDispatcherDisconnectTCPHandle(hDispatcher, SYS_TCP_DISCONNECT);
			case SYS_DSS_CFG_MSG:
                //Sets or Gets Display configuration
				return new CComponentDispatcherDSSConfigHandle(hDispatcher, SYS_DSS_CFG_MSG);
			case SYS_DSS_INIT_MSG:
                //Creates Display Handle and Inits Display Module
				return new CComponentDispatcherDSSInitHandle(hDispatcher, SYS_DSS_INIT_MSG);
			case OMX_PORT_EMPTYTHIS_BUFFER:
                //Free all allocated buffers for given component handle
				return new CComponentDispatcherEmptyThisBufferHandle(hDispatcher, OMX_PORT_EMPTYTHIS_BUFFER);
			case OMX_PORT_FILLTHIS_BUFFER:
                 //Fill all allocated buffers for given component handle
				return new CComponentDispatcherFillThisBufferHandle(hDispatcher, OMX_PORT_FILLTHIS_BUFFER);
			case OMX_GET_CFG_MESSAGE:
                //Fills the configuration structure for given component handle
				return new CComponentDispatcherGetConfigHandle(hDispatcher, OMX_GET_CFG_MESSAGE);
			case OMX_GET_HANDLE_MESSAGE:
                //Init component, dispatcher and callback. Get omx handle and prepare acknowledgment
				return new CComponentDispatcherGetHandleHandle(hDispatcher, OMX_GET_HANDLE_MESSAGE);
			case SYS_GET_IL_CLIENT_VER:
                //Get IL Client version
				return new CComponentDispatcherGetILClientVersionHandle(hDispatcher, SYS_GET_IL_CLIENT_VER);
			case OMX_GET_OMX_VERSION:
                //Get OMX version version for given component
				return new CComponentDispatcherGetOMXVersionHandle(hDispatcher, OMX_GET_OMX_VERSION);
			case OMX_GET_PARAM_MESSAGE:
                //Get parameters of given component
				return new CComponentDispatcherGetParamHandle(hDispatcher, OMX_GET_PARAM_MESSAGE);
			case OMX_GET_COMPONENT_PORTS_MESSAGE:
                //Get Parameters for all ports for given moponent handle
				return new CComponentDispatcherGetPortInfoHandle(hDispatcher, OMX_GET_COMPONENT_PORTS_MESSAGE);
			case SYS_MARSHAL_MEM_FILE_COPY:
                //Copy files inthe marshelling context to buffers
				return new CComponentDispatcherMarshalFileCopyHandle(hDispatcher, SYS_MARSHAL_MEM_FILE_COPY);
			case SYS_MARSHAL_MEM_ALLOC:
                //For given component use ION to allocate buffers for A9
				return new CComponentDispatcherMarshalMemAllocHandle(hDispatcher, SYS_MARSHAL_MEM_ALLOC);
			case SYS_MARSHAL_MEM_ALLOC_TYPE:
				return new CComponentDispatcherMarshalMemAllocTypeHandle(hDispatcher, SYS_MARSHAL_MEM_ALLOC_TYPE);
			case SYS_MARSHAL_MEM_COPY:
				return new CComponentDispatcherMarshalMemCopyHandle(hDispatcher, SYS_MARSHAL_MEM_COPY);
			case SYS_MARSHAL_MEM_DUMP:
				return new CComponentDispatcherMarshalMemDumpHandle(hDispatcher, SYS_MARSHAL_MEM_DUMP);
			case SYS_MARSHAL_MEM_FREE:
				return new CComponentDispatcherMarshalMemFreeHandle(hDispatcher, SYS_MARSHAL_MEM_FREE);
			case SYS_MARSHAL_MEM_READ:
				return new CComponentDispatcherMarshalMemReadHandle(hDispatcher, SYS_MARSHAL_MEM_READ);
			case OMX_ALLOCATE_PORTBUFFERS_MESSAGE:
                //Allocate buffers for given component port using ION, OSAL or OMX methods
				return new CComponentDispatcherPortAllocateBufferHandle(hDispatcher, OMX_ALLOCATE_PORTBUFFERS_MESSAGE);
			case SYS_AUTOBUF_MANAGEMENT:
                //Set bAutoBufferCycle = bEnable
				return new CComponentDispatcherPortAutoBuffCycleHandle(hDispatcher, SYS_AUTOBUF_MANAGEMENT);
			case SYS_PORT_ALLOC_TYPE:
                //Set allocation method
				return new CComponentDispatcherPortConfigAllocTypeHandle(hDispatcher, SYS_PORT_ALLOC_TYPE);
			case OMX_LINK_COMPONENTS_MESSAGE:
                //Link two components' buffers
				return new CComponentDispatcherPortLinkHandle(hDispatcher, OMX_LINK_COMPONENTS_MESSAGE);
			case SYS_REG_PORT_SERVICES:
                //Initiate and start Port Service for given component
				return new CComponentDispatcherPortSetUpServiceHandle(hDispatcher, SYS_REG_PORT_SERVICES);
			case OMX_PORT_USEMRSCTX_BUFFER:
				//return new CComponentDispatcherPortUseMarshalledMemHandle(hDispatcher, OMX_PORT_USEMRSCTX_BUFFER);
				return NULL;
			case OMX_RELEASE_HANDLE_MESSAGE:
                //Free the given component Handle
				return new CComponentDispatcherReleaseHandleHandle(hDispatcher, OMX_RELEASE_HANDLE_MESSAGE);
			case OMX_CFG_MESSAGE:
                //Set configuration for given component
				return new CComponentDispatcherSetConfigHandle(hDispatcher, OMX_CFG_MESSAGE);
			case OMX_PARAM_MESSAGE:
                 //Set parameter for given component
				return new CComponentDispatcherSetParamHandle(hDispatcher, OMX_PARAM_MESSAGE);
			case OMX_SET_ACTUAL_BUFFERS_ON_PORT:
                //Set Port Buffer Count for given component
				return new CComponentDispatcherSetPortBufferCountHandle(hDispatcher, OMX_SET_ACTUAL_BUFFERS_ON_PORT);
			case SYS_CONF_PORT_SERVICE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherConfigPortServicesHandle(hDispatcher, SYS_CONF_PORT_SERVICE);
           case I2C_GET_HANDLE_MESSAGE:
                //Set name preffix of the stream attached to given port
               return new CComponentDispatcherGetI2CHandleHandle(hDispatcher, I2C_GET_HANDLE_MESSAGE);
           case I2C_READ_MESSAGE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherI2CReadHandle(hDispatcher, I2C_READ_MESSAGE);
          case I2C_WRITE_MESSAGE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherI2CWriteHandle(hDispatcher, I2C_WRITE_MESSAGE);
         case I2C_REALEASE_HANDLE_MESSAGE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherReleaseI2CHandleHandle(hDispatcher, I2C_REALEASE_HANDLE_MESSAGE);
#ifdef DVP_ENABLE
         case DVP_ALLOC_KERNEL_NODE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPAllocKernelNodeHandle(hDispatcher, DVP_ALLOC_KERNEL_NODE);
        case DVP_DEINIT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPDeinitHandle(hDispatcher, DVP_DEINIT);
        case DVP_GET_CORE_CAPACITY:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPGetCoreCapacityHandle(hDispatcher, DVP_GET_CORE_CAPACITY);
        case DVP_IMAGE_CONSTRUCT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPImageConstructHandle(hDispatcher, DVP_IMAGE_CONSTRUCT);
        case DVP_IMAGE_FREE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPImageFreeHandle(hDispatcher, DVP_IMAGE_FREE);
        case DVP_IMAGE_INIT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPImageInitHandle(hDispatcher, DVP_IMAGE_INIT);
        case DVP_INIT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPInitHandle(hDispatcher, DVP_INIT);
        case DVP_INIT_NODE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPInitNodeHandle(hDispatcher, DVP_INIT_NODE);
        case DVP_INIT_PARAMS:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPInitParamsHandle(hDispatcher, DVP_INIT_PARAMS);
        case DVP_KERNEL_GRAPH_DEINIT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPKernelGraphDeinitHandle(hDispatcher, DVP_KERNEL_GRAPH_DEINIT);
        case DVP_KERNEL_GRAPH_INIT:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPKernelGraphInitHandle(hDispatcher, DVP_KERNEL_GRAPH_INIT);
        case DVP_KERNEL_NODE_FREE:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPKernelNodeFreeHandle(hDispatcher, DVP_KERNEL_NODE_FREE);
        case DVP_PRINT_PERFORMANCE_GRAPH:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPPrintPerformanceGraphHandle(hDispatcher, DVP_PRINT_PERFORMANCE_GRAPH);
        case DVP_SET_CORE_CAPACITY:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPSetCoreCapacityHandle(hDispatcher, DVP_SET_CORE_CAPACITY);
        case DVP_START_K_GRAPH_PROCESS:
                //Set name preffix of the stream attached to given port
                return new CComponentDispatcherDVPStartKGraphProcessHandle(hDispatcher, DVP_START_K_GRAPH_PROCESS);
#endif
			default:
				return NULL;
		}
	}
};

#endif