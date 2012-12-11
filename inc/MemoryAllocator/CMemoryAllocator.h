#ifndef __CMemoryAllocator__H__
#define __CMemoryAllocator__H__

class CMemoryAllocator
{
public:
	virtual void* alloc(unsigned int length, unsigned int alignment, unsigned int stride) = 0;
	virtual void* alloc2D(unsigned int width,unsigned int height, int fmt, unsigned int* stride) = 0;
	virtual void* getMappedAddr(void* hdl) = 0;
	virtual void* getBufferHdl(void* addr) = 0;
    virtual void* getBufferFd(void* addr) = 0;
	virtual int getBufferStride(void* addr) = 0;
	virtual void free(void* hdl) = 0;
	virtual ~CMemoryAllocator(){};
};

#endif