/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file messages.h
*
* Message types and handler functions
*
* @path WTSD_DucatiMMSW/framework/communication_layer/inc
*
* @rev 00.01
*/
/* -------------------------------------------------------------------------- */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

/****************************************************************
 * INCLUDE FILES
 ***************************************************************/

/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

typedef enum {
    OMX_CFG_MESSAGE,                    //!< 0x00
    OMX_PARAM_MESSAGE,                  //!< 0x01
    OMX_GET_CFG_MESSAGE,                //!< 0x02
    OMX_GET_PARAM_MESSAGE,              //!< 0x03
    OMX_COMMAND_MESSAGE,                //!< 0x04
    OMX_GET_HANDLE_MESSAGE,             //!< 0x05
    OMX_LINK_COMPONENTS_MESSAGE,        //!< 0x06
    OMX_ALLOCATE_PORTBUFFERS_MESSAGE,   //!< 0x07
    OMX_GET_COMPONENT_PORTS_MESSAGE,    //!< 0x08
    OMX_SET_ACTUAL_BUFFERS_ON_PORT,     //!< 0x09
    OMX_RELEASE_HANDLE_MESSAGE,         //!< 0x0A
    OMX_PORT_FILLTHIS_BUFFER,           //!< 0x0B
    OMX_PORT_EMPTYTHIS_BUFFER,          //!< 0x0C
    OMX_PORT_USEMRSCTX_BUFFER,          //!< 0x0D
    OMX_GET_OMX_VERSION,                //!< 0x0E
    OMX_FILL_BUFFER_CAPTURE,            //!< 0x0F
    OMX_FILL_BUFFER_PREVIEW,            //!< 0x10
    OMX_EMPTY_BUFFER_DONE,              //!< 0x11
    OMX_AF_CAF_DONE,                    //!< 0x12

    I2C_MSG_START = 0x0000F000,
    I2C_GET_HANDLE_MESSAGE = I2C_MSG_START, //!< 0x0000F000 :Open I2C device
    I2C_READ_MESSAGE,                       //!< 0x0000F001 :Read from I2C device
    I2C_WRITE_MESSAGE,                      //!< 0x0000F002 :Write to I2C device
    I2C_REALEASE_HANDLE_MESSAGE,            //!< 0x0000F003 :Close I2C device

    SYS_MSG_START   =   0x00F00000,     //!< IL system messages
    SYS_DATA_MSG    =   SYS_MSG_START,  //!< 0x00F00000 :Data transfer message
    SYS_ATTACH_SYS_STREAMER,            //!< 0x00F00001 :IL handles port output - streams to host application
    SYS_DSS_INIT_MSG,                   //!< 0x00F00002 :IL handles port output - displays via DSSCOMP interface
    SYS_PORT_ALLOC_TYPE,                //!< 0x00F00003 :What allocation strategies has IL to use for specific port (OMX_AllocateBuffer, OMX_UseBuffer, TIMM_OSAL_MemAllocExtn, SysMgrAlloc, ...)
    SYS_MARSHAL_MEM_ALLOC,              //!< 0x00F00004 :Allocate memory for data marshalling
    SYS_MARSHAL_MEM_FREE,               //!< 0x00F00005 :Free memory associated with component data marshaling
    SYS_MARSHAL_MEM_COPY,               //!< 0x00F00006 :Memory marshalling to target
    SYS_MARSHAL_MEM_ALLOC_TYPE,         //!< 0x00F00007 :Allocation strategies for memory associated with component data marshaling
    SYS_REG_PORT_SERVICES,              //!< 0x00F00008 :Registers various port related service functions
    SYS_MARSHAL_MEM_DUMP,               //!< 0x00F00009 :Dumps the contents of a marshalled memory to files
    SYS_MARSHAL_MEM_FILE_COPY,          //!< 0x00F0000A :Copies a file to a marshalled memory buffer
    SYS_EVENT_NOTIFICATION,             //!< 0x00F0000B :Interval event notification
    SYS_AUTOBUF_MANAGEMENT,             //!< 0x00F0000C :Auto-buffer managment per port level
    SYS_GET_IL_CLIENT_VER,              //!< 0x00F0000D :Get the IL Client module version
    SYS_MARSHAL_MEM_READ,               //!< 0x00F0000E :Memory marshalling from target
    SYS_CONF_PORT_SERVICE,              //!< 0x00F0000F :Configure ssytem streamer file name prefix
    SYS_DSS_CFG_MSG,                    //!< 0x00F00010 :DSSCOMP configuration message
    SYS_TCP_DISCONNECT,                 //!< 0x00F00011 :Close TCP communucation channel

    DVP_START_MSG    =   0x00F00100,    //!< 0x00F00100 : DVP_START_MSG
    DVP_ALLOC_KERNEL_NODE,              //!< 0x00F00101 : Allocate kernel nodes
    DVP_DEINIT,                         //!< 0x00F00102 : Deinit DVP
    DVP_GET_CORE_CAPACITY,              //!< 0x00F00103 : Get Core Capacity
    DVP_IMAGE_ALLOC,                    //!< 0x00F00104 : Allocate Image
    DVP_IMAGE_CONSTRUCT,                //!< 0x00F00105 : Costruct image
    DVP_IMAGE_FREE,                     //!< 0x00F00106 : Free Image
    DVP_IMAGE_INIT,                     //!< 0x00F00107 : Image Init
    DVP_INIT,                           //!< 0x00F00108 : Init DVP
    DVP_INIT_NODE,                      //!< 0x00F00109 : Init Node
    DVP_INIT_PARAMS,                    //!< 0x00F0010A : Init Params
    DVP_KERNEL_GRAPH_DEINIT,            //!< 0x00F0010B : Kernel graph deinit - relese DVP handle
    DVP_KERNEL_GRAPH_INIT,              //!< 0x00F0010C : Kernel graph init - get DVP handle
    DVP_KERNEL_NODE_FREE,               //!< 0x00F0010D : Free Kernel Nodes
    DVP_PRINT_PERFORMANCE_GRAPH,        //!< 0x00F0010E : Print Graph Perfomance
    DVP_SET_CORE_CAPACITY,              //!< 0x00F0010F : Set Core Capacity
    DVP_START_K_GRAPH_PROCESS,          //!< 0x00F00110 : Start Kernel Grapth process

    LAST_MESSAGE
} systemMesages_t;

typedef enum {
    MSG_STATUS_OK,          //!< 0x0
    MSG_STATUS_ERROR,       //!< 0x1
    MSG_STATUS_DATA_FULL,   //!< 0x2
    MSG_STATUS_DATA_START,  //!< 0x3
    MSG_STATUS_DATA_MORE,   //!< 0x4
    MSG_STATUS_DATA_END,    //!< 0x5
    MSG_STATUS_ENABLE,      //!< 0x6
    MSG_STATUS_DISABLE,     //!< 0x7
    MSG_STATUS_GET,         //!< 0x8
    MSG_STATUS_SET,         //!< 0x9
    MSG_STATUS_WRONG_PARAM = 0x100,     //!< 0x100
    MSG_STATUS_COUNT
} messageStatuses_t;

typedef enum {
    EVNT_OMX_PORT_BUFFER_EVENT,     //!< 0x0
    LAST_EVENT
} ILModSystemEvents_t;

typedef enum {
    EVNT_STATUS_PORT_EMPTYBUFFER_DONE,  //!< 0x0
    EVNT_STATUS_PORT_FILLBUFFER_DONE,   //!< 0x1
    EVNT_STATUS_COUNT
} ILModEventStatuses_t;

#endif // __MESSAGES_H__
