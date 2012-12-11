#ifndef __COMXPortServiceFaceRectDraw__H__
#define __COMXPortServiceFaceRectDraw__H__

#include "inc/lib_comm.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"

#define FACE_TRACKING_OUTLINE_COLOR_WHITE   (0xFF80FF80)
#define FACE_TRACKING_OUTLINE_COLOR_BLACK   (0x00800080)
#define FACE_TRACKING_OUTLINE_COLOR_GREY    (0x80808080)
#define FACE_TRACKING_OUTLINE_COLOR_RED     (0x4CFF4C54)
#define FACE_TRACKING_OUTLINE_COLOR_GREEN   (0x9515952B)
#define FACE_TRACKING_OUTLINE_COLOR_BLUE    (0x1D6B1DFF)
#define FACE_TRACKING_OUTLINE_COLOR_YELLOW  (0xE194E100)
#define FACE_TRACKING_OUTLINE_COLOR_PURPLE  (0x69AE69D4)

typedef struct {
    TIMM_OSAL_S16  left;   //!< Horizontal coordinate of left side
    TIMM_OSAL_S16  top;    //!< Vertical coordinate of top side
    TIMM_OSAL_S16  right;  //!< Horizontal coordinate of right side
    TIMM_OSAL_S16  bottom; //!< Vertical coordinate of bottom side
} draw_rect_t;

typedef struct {
    TIMM_OSAL_U32 nStride;
    TIMM_OSAL_U32 nWidth;
    TIMM_OSAL_U32 nHeight;
    TIMM_OSAL_U32 nBuffWidth;
    TIMM_OSAL_U32 nBuffHeight;
    TIMM_OSAL_U32 nFormatType;
}FaceRectDrawData_t;

#define CORNER_SIZE     (8)
#define CORNER_DIVISOR  (5)
#define CORNER_MIN_SIZE (8)
#define COORD_ALIGN     (1)
#define EVEN_MASK       ((TIMM_OSAL_U32)~COORD_ALIGN)

class COMXPortServiceFaceRectDraw : public COMXPortService
{
public:
	COMXPortServiceFaceRectDraw(int id):COMXPortService(id){};
	TIMM_OSAL_ERRORTYPE Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize);
	TIMM_OSAL_ERRORTYPE Deinit();
	//bool CheckActivity();
	TIMM_OSAL_ERRORTYPE Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType);
    TIMM_OSAL_ERRORTYPE ConfigPortService();
private:
	FaceRectDrawData_t* faceRectD;

	int yuvDrawReticule (draw_rect_t* rc_face, void* img_data, int line_size, unsigned int line_color, unsigned int thick_line);
};

#endif