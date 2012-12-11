#include "inc/PortService/Implementation/COMXPortServiceFaceRectDraw.h"

int COMXPortServiceFaceRectDraw::yuvDrawReticule (draw_rect_t* rc_face, void* img_data, int line_size, unsigned int line_color, unsigned int thick_line)
{
    unsigned int  *ptr_lrt;  // This pointer is used for drawing left to right top line
    unsigned int  *ptr_rlt;  // This pointer is used for drawing right to left top line
    unsigned int  *ptr_lrb;  // This pointer is used for drawing left to right bottom line
    unsigned int  *ptr_rlb;  // This pointer is used for drawing right to left bottom line
    unsigned int  *ptr_tbl;  // This pointer is used for drawing top to bottom left line
    unsigned int  *ptr_btl;  // This pointer is used for drawing bottom to top left line
    unsigned int  *ptr_tbr;  // This pointer is used for drawing top to bottom right line
    unsigned int  *ptr_btr;  // This pointer is used for drawing bottom to top right line
    unsigned int  count;
    unsigned int  cnt;

    unsigned int  corner_size;

    unsigned int  left_top;
    unsigned int  right_top;
    unsigned int  left_bottom;
    unsigned int  right_bottom;

    corner_size = ((rc_face->right - rc_face->left + 1) >> CORNER_DIVISOR) * CORNER_SIZE;
    if(CORNER_MIN_SIZE > corner_size)
    {
        corner_size = CORNER_MIN_SIZE;
    }
    corner_size &= EVEN_MASK; // Must be even

    left_top = ((((rc_face->left) + rc_face->top * line_size) >> 1) & EVEN_MASK);
    right_top = ((((rc_face->right) + rc_face->top * line_size) >> 1) & EVEN_MASK);
    left_bottom = ((((rc_face->left) + rc_face->bottom * line_size) >> 1) & EVEN_MASK);
    right_bottom = ((((rc_face->right) + rc_face->bottom * line_size) >> 1) & EVEN_MASK);

    for(cnt = 0; cnt < thick_line; cnt++)
    {
        ptr_lrt = ((unsigned int*)img_data + (left_top + cnt + (line_size >> 1)*(cnt << 1)));
        ptr_rlt = ((unsigned int*)img_data + (right_top - cnt + (line_size >> 1)*(cnt << 1)));
        ptr_lrb = ((unsigned int*)img_data + (left_bottom + cnt - (line_size >> 1)*(cnt << 1)));
        ptr_rlb = ((unsigned int*)img_data + (right_bottom - cnt - (line_size >> 1)*(cnt << 1)));
        ptr_tbl = ptr_lrt;
        ptr_btl = ptr_lrb;
        ptr_tbr = ptr_rlt;
        ptr_btr = ptr_rlb;

        if (corner_size == 0)
        {
            break;
        }

        for (count = 0; count <= ((corner_size >> 1) - (cnt)); count++)
        {

            *ptr_lrt = line_color;
            *(ptr_lrt + (line_size >> 1)) = line_color;
            ptr_lrt++;

           *ptr_rlt = line_color;
            *(ptr_rlt + (line_size >> 1)) = line_color;
            ptr_rlt--;

           *ptr_lrb = line_color;
            *(ptr_lrb + (line_size >> 1)) = line_color;
            ptr_lrb++;

            *ptr_rlb = line_color;
            *(ptr_rlb + (line_size >> 1)) = line_color;
            ptr_rlb--;

            *ptr_tbl = line_color;
            ptr_tbl += line_size >> 1;
            *ptr_tbl = line_color;
            ptr_tbl += line_size >> 1;

            *ptr_btl = line_color;
            ptr_btl -= line_size >> 1;
            *ptr_btl = line_color;
            ptr_btl -= line_size >> 1;

            *ptr_tbr = line_color;
            ptr_tbr += line_size >> 1;
            *ptr_tbr = line_color;
            ptr_tbr += line_size >> 1;

            *ptr_btr = line_color;
            ptr_btr -= line_size >> 1;
            *ptr_btr = line_color;
            ptr_btr -= line_size >> 1;

        }
    }

    return (0);
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFaceRectDraw::Init(COMXComponent* ipComp, unsigned int iAssociatedPort, void* pServiceHeader, int nServiceHeadSize)
{
	COMXPortHandle* portData;
    OMX_CONFIG_RECTTYPE tFrameDim;

	TIMM_OSAL_ERRORTYPE err = this->COMXPortService::Init(ipComp, iAssociatedPort, pServiceHeader, nServiceHeadSize);
	if(err != TIMM_OSAL_ERR_NONE)
	{
		return err;
	}

    faceRectD = (FaceRectDrawData_t*)malloc(sizeof(FaceRectDrawData_t));

    if(faceRectD == NULL)
	{
        printf("Face detect: insufficient memory space.\n");
        return TIMM_OSAL_ERR_ALLOC;
    }

	pComp = ipComp;
	associatedPort = iAssociatedPort;

    OMX_INIT_STRUCT_PTR(&tFrameDim, OMX_CONFIG_RECTTYPE);
    tFrameDim.nPortIndex = associatedPort;

	portData = pComp->GetPortData(associatedPort);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data\n", associatedPort);
		return TIMM_OSAL_ERR_OMX;
	}

    if(portData->tPortDef.eDomain == OMX_PortDomainVideo)
	{
        faceRectD->nStride = portData->tPortDef.format.video.nStride;
        faceRectD->nWidth = portData->tPortDef.format.video.nFrameWidth;
        faceRectD->nHeight = portData->tPortDef.format.video.nFrameHeight;

        if(portData->tPortDef.format.video.eColorFormat != OMX_COLOR_FormatCbYCrY)
		{
            faceRectD->nFormatType = 1;
        }
		else
		{
            faceRectD->nFormatType = 0;
        }

        if (OMX_ErrorNone == pComp->GetParam((OMX_INDEXTYPE)OMX_TI_IndexParam2DBufferAllocDimension, (unsigned char*)&tFrameDim))
		{
            faceRectD->nBuffWidth = tFrameDim.nWidth;
            faceRectD->nBuffHeight = tFrameDim.nHeight;
        }
		else
		{
            faceRectD->nBuffWidth = faceRectD->nWidth;
            faceRectD->nBuffHeight = faceRectD->nHeight;
        }
    }
	else
	{
        printf("Face detect: Unsupported port domain!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFaceRectDraw::Deinit()
{
    if(faceRectD == NULL)
	{
        printf("Face detect: service not initialized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    free(faceRectD);
	faceRectD = NULL;

    return this->COMXPortService::Deinit();
}

TIMM_OSAL_ERRORTYPE COMXPortServiceFaceRectDraw::ConfigPortService()
{
    return TIMM_OSAL_ERR_NONE;

}

TIMM_OSAL_ERRORTYPE COMXPortServiceFaceRectDraw::Execute(OMX_BUFFERHEADERTYPE* pBufHdr, memAllocStrat_t eAllocType)
{
    if(!StartExecute())
	{
        printf("FaceRectDraw: service not STARTED!\n");
		return TIMM_OSAL_ERR_UNKNOWN;
	}

    TIMM_OSAL_ERRORTYPE err = TIMM_OSAL_ERR_NONE;

    OMX_TI_PLATFORMPRIVATE* pvtData = (OMX_TI_PLATFORMPRIVATE*)(pBufHdr->pPlatformPrivate);

    if(pvtData->nMetaDataSize > 0 && pvtData->pMetaDataBuffer != NULL)
	{

        if(faceRectD == NULL)
		{
            printf("Face detect: service not initialized!\n");
			StopExecute();
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        OMX_OTHER_EXTRADATATYPE* extData = (OMX_OTHER_EXTRADATATYPE*)pvtData->pMetaDataBuffer;

        while(((TIMM_OSAL_U8*)extData < (TIMM_OSAL_U8*)pvtData->pMetaDataBuffer + pvtData->nMetaDataSize) &&
               ((OMX_EXT_EXTRADATATYPE)extData->eType != OMX_FaceDetection) &&
               (extData->eType != OMX_ExtraDataNone))
		{
            extData = (OMX_OTHER_EXTRADATATYPE*) ((TIMM_OSAL_U8*)extData + extData->nSize);
        }

        if(((TIMM_OSAL_U8*)extData < (TIMM_OSAL_U8*)pvtData->pMetaDataBuffer + pvtData->nMetaDataSize) &&
            ((OMX_EXT_EXTRADATATYPE)extData->eType == OMX_FaceDetection))
		{
            OMX_FACEDETECTIONTYPE* fcsData = (OMX_FACEDETECTIONTYPE*)extData->data;

            if(fcsData->ulFaceCount > 0)
			{
                if(fcsData->ulFaceCount > 34)
				{
                    printf("Face detect: bogust face count %d\n", fcsData->ulFaceCount);
					StopExecute();
                    return TIMM_OSAL_ERR_UNKNOWN;
                }

                MMS_IL_PRINT("Face detect: faces detected %d\n", fcsData->ulFaceCount);

                draw_rect_t tmpRect;

                OMX_TI_FACERESULT* fcsPos = fcsData->tFacePosition;
                void* img_buff = NULL;

                if (faceRectD->nFormatType == 1)
				{
                    int offstX, offstY;

                    offstX = pBufHdr->nOffset%(faceRectD->nStride);
                    offstY = pBufHdr->nOffset/(faceRectD->nStride);

                    img_buff = (OMX_U8*)pBufHdr->pBuffer + faceRectD->nBuffHeight*faceRectD->nStride + faceRectD->nStride*(offstY/2) + offstX/2;
                }
				else if(faceRectD->nFormatType == 0)
				{
                    img_buff = (OMX_U8*)pBufHdr->pBuffer + pBufHdr->nOffset;
                }

                for(int iter = 0; iter < fcsData->ulFaceCount; iter++)
				{
                    if(fcsPos[iter].nWidth > faceRectD->nWidth)
					{
                        printf("Face detect: bogust face width %lu\n", fcsPos[iter].nWidth);
						StopExecute();
                        return TIMM_OSAL_ERR_UNKNOWN;
                    }
                    if(fcsPos[iter].nHeight > faceRectD->nHeight)
					{
                        printf("Face detect: bogust face height %lu\n", fcsPos[iter].nHeight);
						StopExecute();
                        return TIMM_OSAL_ERR_UNKNOWN;
                    }
                    tmpRect.left = fcsPos[iter].nLeft;
                    tmpRect.top = fcsPos[iter].nTop;
                    tmpRect.right = fcsPos[iter].nLeft + fcsPos[iter].nWidth;
                    tmpRect.bottom = fcsPos[iter].nTop + fcsPos[iter].nHeight;

                    if(tmpRect.left < 0)
					{
                        tmpRect.left = 0;
                    }
					else if((unsigned int) tmpRect.left > faceRectD->nWidth - fcsPos[iter].nWidth)
					{
                        tmpRect.left = faceRectD->nWidth - fcsPos[iter].nWidth;
                    }

                    if((unsigned int) tmpRect.right >= faceRectD->nWidth)
					{
                        tmpRect.right = faceRectD->nWidth - 1;
                    }
					else if((unsigned int) tmpRect.right < fcsPos[iter].nWidth)
					{
                        tmpRect.right = fcsPos[iter].nWidth;
                    }

                    if(tmpRect.top < 0)
					{
                        tmpRect.top = 0;
                    }
					else if((unsigned int) tmpRect.top > faceRectD->nHeight - fcsPos[iter].nHeight)
					{
                        tmpRect.top = faceRectD->nHeight - fcsPos[iter].nHeight;
                    }

                    if((unsigned int) tmpRect.bottom >= faceRectD->nHeight)
					{
                        tmpRect.bottom = faceRectD->nHeight - 1;
                    }
					else if((unsigned int) tmpRect.bottom < fcsPos[iter].nHeight)
					{
                        tmpRect.bottom = fcsPos[iter].nHeight;
                    }

                    MMS_IL_PRINT("Face detect: top %d left %d right %d bottom %d\n", tmpRect.top, tmpRect.left, tmpRect.right, tmpRect.bottom);
                    if(faceRectD->nFormatType == 0)
					{
                        yuvDrawReticule (&tmpRect, img_buff, faceRectD->nStride/2, FACE_TRACKING_OUTLINE_COLOR_GREEN, 1);
                    }
					else if (faceRectD->nFormatType == 1)
					{
                        tmpRect.left /= 2;
                        tmpRect.right /= 2;
                        tmpRect.top /= 2;
                        tmpRect.bottom /= 2;

                        yuvDrawReticule (&tmpRect, img_buff, faceRectD->nStride/2, FACE_TRACKING_OUTLINE_COLOR_GREEN, 1);
                    }
                }
            }
        }
		else
		{
            MMS_IL_PRINT("Face detect: no face detection data found in frame extra data!\n");
        }
    }

	StopExecute();
    return err;
}