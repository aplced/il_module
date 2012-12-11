STLPORT_FORCE_REBUILD := true

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    src/Communication/I2C/CCommunicationI2C.cpp\
    src/Communication/Channel/Implementation/CCommunicationChannelImplementation.cpp \
    src/Communication/Media/Tcp/CCommunicationMediaTcp.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetConfigHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetConfigHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetParamHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetParamHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherCommandHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetHandleHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseHandleHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortLinkHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAllocateBufferHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortConfigAllocTypeHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortSetUpServiceHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAutoBuffCycleHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetPortInfoHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetPortBufferCountHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherFillThisBufferHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherEmptyThisBufferHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetOMXVersionHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSInitHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSConfigHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherConfigPortServicesHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetILClientVersionHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemFreeHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemAllocTypeHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemCopyHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemReadHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalMemDumpHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherMarshalFileCopyHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDisconnectTCPHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetI2CHandleHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseI2CHandleHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CReadHandle.cpp \
    src/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherI2CWriteHandle.cpp \
    src/ComponentDispatcher/OMXComponent/COMXComponent.cpp \
    src/ComponentDispatcher/OMXComponent/COMXBuffDoneManager.cpp \
    src/ComponentDispatcher/COMXComponentDispatcher.cpp \
    src/Display/DSS/CDisplayDSS.cpp \
    src/PortService/COMXPortServiceManager.cpp \
    src/PortService/COMXPortService.cpp \
    src/PortService/Implementation/COMXPortServiceExtractMetad.cpp \
    src/PortService/Implementation/COMXPortServiceFaceRectDraw.cpp \
    src/PortService/Implementation/COMXPortServiceFrameOIP.cpp \
    src/PortService/Implementation/COMXPortServiceFrameRate.cpp \
    src/PortService/Implementation/COMXPortServiceMetadOIP.cpp \
    src/PortService/Implementation/COMXPortServiceSaveBuffer.cpp \
    src/PortService/Implementation/COMXPortServiceStreamBuffer.cpp \
    src/PortService/Implementation/COMXPortServiceAFStatusCallBack.cpp \
    src/MemoryAllocator/Ion/CMemoryAllocatorIon.cpp \
    src/TransferBuffer/Save2File/CTransferBufferSave2File.cpp \
    src/TransferBuffer/Stream2Host/CTransferBufferStream2Host.cpp \
    src/data_server.cpp \
    src/main.cpp

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/inc \
    bionic/libc/kernel/common/video \
    hardware/ti/domx/mm_osal/inc \
    hardware/ti/domx/omx_core/inc \
    hardware/ti/omap4xxx/ion \
    system/core/include/ion \
    frameworks/base/include/surfaceflinger \
    ndk/sources/cxx-stl/stlport/stlport

LOCAL_SHARED_LIBRARIES:= \
    libdl \
    libui \
    libbinder \
    libutils \
    libcutils \
    libtiutils \
    libOMX_Core \
    libmm_osal \
    libion  \
    libstlport

#LOCAL_CFLAGS := -fno-short-enums
LOCAL_CFLAGS += -Wall -fno-short-enums -O0 -g -D___ANDROID___

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE:= omx_il_module.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

ifeq ($(TARGET_BOARD_PLATFORM),omap4)
    LOCAL_CFLAGS += -DTARGET_OMAP4
endif
include $(BUILD_HEAPTRACKED_EXECUTABLE)
#include $(BUILD_EXECUTABLE)

