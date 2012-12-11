#ifndef __CDisplayFactory__H__
#define __CDisplayFactory__H__

#include "inc/Display/DSS/CDisplayDSS.h"
//#include "inc/Display/SurfaceFlinger/CDisplaySurfaceFlinger.h"

enum DisplayTypes
{
	DSS_DisplayType,
	SurfaceFlinger_DisplayType
};

class CDisplayFactory
{
public:
	static CDisplay* CreateDisplayHandle(DisplayTypes displayT)
	{
		switch(displayT)
		{
			case DSS_DisplayType:
				return new CDisplayDSS();
		//	case SurfaceFlinger_DisplayType:
		//		return new CDisplaySurfaceFlinger();
			default:
				return NULL;
		}
	}
};

#endif