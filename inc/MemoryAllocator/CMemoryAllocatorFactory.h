#ifndef __CMemoryAllocatorFactory__H__
#define __CMemoryAllocatorFactory__H__

#include "inc/MemoryAllocator/CMemoryAllocator.h"
#include "inc/MemoryAllocator/Ion/CMemoryAllocatorIon.h"

enum MemoryAllocatorTypes
{
	ION_MemoryAllocatorType
};//Currently supported only ION

class CMemoryAllocatorFactory
{
public:
	static CMemoryAllocator* CreateMemoryAllocatorHandle(MemoryAllocatorTypes memAllocT)
	{
		static CMemoryAllocatorIon* ionHdl = NULL;
		switch(memAllocT)
		{
			case ION_MemoryAllocatorType:
				if(ionHdl == NULL)
				{  //Allocate new MemoryAllocatorHandle
                    //for ION_MemoryAllocatorType
					ionHdl = new CMemoryAllocatorIon();
				}
				return ionHdl;
			default:
				return NULL;
		}
	}
};

#endif