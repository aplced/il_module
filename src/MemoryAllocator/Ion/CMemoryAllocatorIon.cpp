#include "inc/MemoryAllocator/Ion/CMemoryAllocatorIon.h"
//#define MMS_IL_PRINT(ARGS...)
void* CMemoryAllocatorIon::alloc(unsigned int length, unsigned int alignment, unsigned int stride)
{
    MMS_IL_PRINT("length %d; alignment %d\n", length, alignment);
    return ION_Alloc(length, alignment, stride);
}

void* CMemoryAllocatorIon::alloc2D(unsigned int width,unsigned int height, int fmt, unsigned int* stride)
{
    MMS_IL_PRINT("width %d; height %d; fmt %d; *stride %d\n", width, height, fmt, *stride);
    return ION_Alloc_2D(width, height, fmt, stride);
}

void* CMemoryAllocatorIon::getMappedAddr(void* map_fd)
{
    MMS_IL_PRINT("map_fd 0x%X\n", (unsigned int)map_fd);
    return ION_GetMappedAddr(map_fd);
}

int CMemoryAllocatorIon::getBufferStride(void* map_fd)
{
    MMS_IL_PRINT("map_fd 0x%X\n", (unsigned int)map_fd);
    return ION_GetStride(map_fd);
}

void* CMemoryAllocatorIon::getBufferHdl(void* map_fd)
{
    MMS_IL_PRINT("map_fd 0x%08X\n", (unsigned int)map_fd);
    return ION_GetHdl(map_fd);
}
void* CMemoryAllocatorIon::getBufferFd(void* addr)
{
    MMS_IL_PRINT("addr 0x%08X\n", (unsigned int)addr);
    return ION_GetFd(addr);
}

void CMemoryAllocatorIon::free(void* map_fd)
{
    MMS_IL_PRINT("addr 0x%08X\n", (unsigned int)map_fd);
    ION_Free(map_fd);
}

ION_Buffer_t* CMemoryAllocatorIon::new_buf(void)
{
    if (ionFd == 0)
    {
        MMS_IL_PRINT("Opening ION device\n");
        ionFd = ion_open();
        if (ionFd < 0)
        {
            MMS_IL_PRINT("Opening ION device failed\n");
            return NULL;
        }
    }

    ION_Buffer_t* buf = (ION_Buffer_t*)malloc(sizeof(ION_Buffer_t));
    MMS_IL_PRINT("Inserting buf in vector\n");
    ionHdls.push_back(buf);

    MMS_IL_PRINT("Returning new buf 0x%08X\n", (unsigned int)buf);
    return buf;
}

TIMM_OSAL_ERRORTYPE CMemoryAllocatorIon::delete_buf(void* map_fd)
{
    ION_Buffer_t* buf = NULL;

    for(int i = 0; i < ionHdls.size(); i++)
    {
        if(ionHdls[i]->map_fd == (int)map_fd)
        {
            buf = ionHdls[i];
            ionHdls.erase(ionHdls.begin() + i);
            break;
        }
    }

    if(buf == NULL)
    {
        MMS_IL_PRINT("Cant find ION handle 0x%08X\n", (int)map_fd);
        return TIMM_OSAL_ERR_MSG_TYPE_NOT_FOUND;
    }

    if (buf->addr != NULL)
    {
        munmap(buf->addr, buf->len);
    }

    if (buf->hdl != NULL)
    {
        ion_free(ionFd, buf->hdl);
    }

    free(buf);
    return TIMM_OSAL_ERR_NONE;
}

void* CMemoryAllocatorIon::ION_Alloc(unsigned int length, unsigned int alignment, unsigned int stride)
{
    int ret;
    //int map_fd;
    ION_Buffer_t *buf = new_buf();

    if (buf == NULL)
    {
        MMS_IL_PRINT("Allocating new ION buffer failed\n");
        return NULL;
    }
    //use ION lib for buffer allocation
    ret = ion_alloc(ionFd, length, alignment, ION_HEAP_CARVEOUT_MASK, &buf->hdl);

    buf->stride = 0;
   // buf->stride = stride;

    if (ret)
    {
        MMS_IL_PRINT("Allocating new ION buffer failed\n");
        delete_buf(buf->addr);
        return NULL;
    }

    buf->len = length;
    ret = ion_map(ionFd, buf->hdl, buf->len, PROT_READ | PROT_WRITE, MAP_SHARED, 0, (unsigned char **) &buf->addr, &buf->map_fd);
    // close(map_fd);
    if (ret)
    {
        MMS_IL_PRINT("Mapping new ION buffer failed\n");
        delete_buf(buf->addr);
        return NULL;
    }

    MMS_IL_PRINT("Allocated ION buffer handle %p, length %d\n", buf->hdl, length);
    //return (void*) buf->hdl;
    return (void*) buf->map_fd;
}

void* CMemoryAllocatorIon::ION_Alloc_2D(unsigned int width,unsigned int height, int fmt,unsigned int* stride)
{
    int ret;
    //int map_fd;
    ION_Buffer_t* buf = new_buf();

    if (buf == NULL)
    {
        MMS_IL_PRINT("Allocating new ION 2D buffer failed\n");
        return NULL;
    }

    ret = ion_alloc_tiler(ionFd, width, height, fmt, OMAP_ION_HEAP_TILER_MASK, &buf->hdl, &buf->stride);

    *stride = buf->stride;

    if (ret)
    {
        MMS_IL_PRINT("Allocating new ION 2D buffer failed\n");
        delete_buf(buf->addr);
        return NULL;
    }

    buf->len = height * (*stride);
    ret = ion_map(ionFd, buf->hdl, buf->len, PROT_READ | PROT_WRITE, MAP_SHARED, 0, (unsigned char**) &buf->addr, &buf->map_fd);

    //close(map_fd);
    if (ret)
    {
        MMS_IL_PRINT("Mapping new ION 2D buffer failed\n");
        delete_buf(buf->addr);
        return NULL;
    }

    MMS_IL_PRINT("Allocated ION 2D buffer handle %p, %d x %d, stride %d\n", buf->hdl, width, height, *stride);
    //return (TIMM_OSAL_PTR) buf->addr;
    return (void*) buf->map_fd;
}

void CMemoryAllocatorIon::ION_Free(void* map_fd)
{
    if (TIMM_OSAL_ERR_NONE != delete_buf(map_fd))
    {
        MMS_IL_PRINT("Buffer addr %p not found in ION buffers list\n", map_fd);
    }
    else
    {
        MMS_IL_PRINT("Freed ION buffer %p\n", map_fd);
    }
    close((int)map_fd);
}

void* CMemoryAllocatorIon::ION_GetMappedAddr(void* map_fd)
{
    ION_Buffer_t* buf = NULL;

    for(unsigned int i = 0; i < ionHdls.size(); i++)
    {
        if(ionHdls[i]->map_fd == (int)map_fd)
        {
            buf = ionHdls[i];
            break;
        }
    }

    if(buf == NULL)
    {
        MMS_IL_PRINT("Cant find ION fd 0x%08X\n",(int) map_fd);
        return NULL;
    }
    else
    {
        return buf->addr;
    }
}

int CMemoryAllocatorIon::ION_GetStride(void* map_fd)
{
    ION_Buffer_t* buf = NULL;

    for(unsigned int i = 0; i < ionHdls.size(); i++)
    {
        if(ionHdls[i]->map_fd == (int)map_fd)
        {
            buf = ionHdls[i];
            break;
        }
    }

    if(buf == NULL)
    {
        MMS_IL_PRINT("Cant find ION fd 0x%08X\'s handlen\n", (int)map_fd);
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    else
    {
        return buf->stride;
    }
}

void* CMemoryAllocatorIon::ION_GetHdl(void* addr)
{
    ION_Buffer_t* buf = NULL;

    for(unsigned int i = 0; i < ionHdls.size(); i++)
    {
        if(ionHdls[i]->addr == addr)
        {
            buf = ionHdls[i];
            break;
        }
    }

    if(buf == NULL)
    {
        MMS_IL_PRINT("Cant find ION address 0x%08X\'s handlen\n", (unsigned int)addr);
        return NULL;
    }
    else
    {
        return buf->hdl;
    }
}

void* CMemoryAllocatorIon::ION_GetFd(void* addr)
{
    ION_Buffer_t* buf = NULL;

    for(unsigned int i = 0; i < ionHdls.size(); i++)
    {
        if(ionHdls[i]->addr == addr)
        {
            buf = ionHdls[i];
            break;
        }
    }

    if(buf == NULL)
    {
        MMS_IL_PRINT("Cant find ION address 0x%08X\'s handlen\n", (unsigned int)addr);
        return NULL;
    }
    else
    {
        return (void*)buf->map_fd;
    }
}