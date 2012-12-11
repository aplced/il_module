#ifndef __CMemoryAllocatorIon__H__
#define __CMemoryAllocatorIon__H__

#include <stdlib.h>
#include <vector>
#include "inc/MemoryAllocator/CMemoryAllocator.h"

#include <sys/types.h>
#include <timm_osal_types.h>
#include "inc/lib_comm.h"

extern "C" {
#include <ion.h>
};

using namespace std;

typedef struct {
    struct ion_handle* hdl;
	void* addr;
	unsigned int stride;
	size_t len;
    int map_fd;
} ION_Buffer_t;

class CMemoryAllocatorIon : public CMemoryAllocator
{
public:
	void* alloc(unsigned int length, unsigned int alignment, unsigned int stride);
	void* alloc2D(unsigned int width,unsigned int height, int fmt, unsigned int* stride);
	void* getMappedAddr(void* hdl);
	int getBufferStride(void* addr);
	void* getBufferHdl(void* addr);
    void* getBufferFd(void* addr);
	void free(void* hdl);

private:
	int ionFd;
	vector<ION_Buffer_t*> ionHdls;

    ION_Buffer_t* new_buf(void);
    TIMM_OSAL_ERRORTYPE delete_buf(void* hdl);
	void* ION_Alloc(unsigned int length, unsigned int alignment, unsigned int stride);
	void* ION_Alloc_2D(unsigned int width,unsigned int height, int fmt,unsigned int* stride);
	void ION_Free(void* hdl);
	void* ION_GetMappedAddr(void* hdl);
	int ION_GetStride(void* addr);
	void* ION_GetHdl(void* addr);
    void* ION_GetFd(void* addr);
};

#endif