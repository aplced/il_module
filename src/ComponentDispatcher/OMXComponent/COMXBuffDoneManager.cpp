#include "inc/ComponentDispatcher/OMXComponent/COMXBuffDoneManager.h"
#include "inc/ComponentDispatcher/OMXComponent/COMXComponent.h"
//#define MMS_IL_PRINT(ARGS...)

static void* OMX_BufferDone_thread(void* arg);

COMXBuffDoneManager::COMXBuffDoneManager(COMXComponent* iOMXComp)
{
	omxComp = iOMXComp;
	managmentInited = false;
}

COMXBuffDoneManager::~COMXBuffDoneManager()
{
	Stop();
}

TIMM_OSAL_ERRORTYPE COMXBuffDoneManager::Start()
{
	if(!managmentInited)
	{
		head = 0;
		tail = 0;
		size = 10;
		items = 0;

		thExit = false;
		pBufDone = (OMXbufferDone_t*)malloc(sizeof(OMXbufferDone_t)*size);
        if (!pBufDone)
        {
          MMS_IL_PRINT("OMXBufferDone buffer allocation failed\n");
          return TIMM_OSAL_ERR_ALLOC;
        }
		sem_init(&mutex, 0, 1);
		sem_init(&wait4Buffer, 0, 0);
		if (pthread_create(&thread, NULL, OMX_BufferDone_thread, (void*)this))
        {
			MMS_IL_PRINT("OMX BufferDone CB thread creation failed\n");
			sem_destroy(&wait4Buffer);
			sem_destroy(&mutex);
			free(pBufDone);
			return TIMM_OSAL_ERR_UNKNOWN;
		}

		managmentInited = true;
	}

	return TIMM_OSAL_ERR_NONE;
}

void COMXBuffDoneManager::Stop()
{
	if(managmentInited)
	{
		thExit = OMX_TRUE;
		sem_post(&wait4Buffer);
        //MMS_IL_PRINT("Joining thread!\n");
		pthread_join(thread, NULL);
        //MMS_IL_PRINT("thread joined!\n");
		sem_destroy(&wait4Buffer);
		sem_destroy(&mutex);
		free(pBufDone);

		managmentInited = false;
	}
}

TIMM_OSAL_ERRORTYPE COMXBuffDoneManager::GrowBuffDoneCircularBuffer()
{
	OMXbufferDone_t* tmp = (OMXbufferDone_t*)malloc(sizeof(OMXbufferDone_t)*size*2);
    if (!tmp)
    {
        MMS_IL_PRINT("OMXBufferDone buffer allocation failed\n");
        return TIMM_OSAL_ERR_ALLOC;
    }
	memcpy(tmp, pBufDone, sizeof(OMXbufferDone_t)*size);
	free(pBufDone);
	pBufDone = tmp;
	size *= 2;
    return TIMM_OSAL_ERR_NONE;
}

void COMXBuffDoneManager::Push(omxBufDoneCBType_t cbType, OMX_BUFFERHEADERTYPE* buffer)
{
	sem_wait(&mutex);

	pBufDone[head].cbType = cbType;
	pBufDone[head].pBuffer = buffer;

	items++;

	if(items == size)
	{
        if (TIMM_OSAL_ERR_ALLOC == GrowBuffDoneCircularBuffer())
            MMS_IL_PRINT("CB is full no more memory for new buffers!\n");
	}

	head = (head + 1) % size;
	if(head == tail)
	{
		//yadayadayada
	}

	sem_post(&mutex);

	sem_post(&wait4Buffer);
}

OMXbufferDone_t* COMXBuffDoneManager::Pop()
{
	OMXbufferDone_t* returnee = NULL;

	sem_wait(&wait4Buffer);

	sem_wait(&mutex);

	if(items > 0)
	{
		returnee = &pBufDone[tail];
		tail = (tail + 1) % size;
		items--;
	}

	sem_post(&mutex);
	return returnee;
}

COMXComponent* COMXBuffDoneManager::GetOMXComp()
{
	return omxComp;
}

bool COMXBuffDoneManager::ThreadExit()
{
	return thExit;
}

static void* OMX_BufferDone_thread(void* arg)
{
	COMXBuffDoneManager* thData = (COMXBuffDoneManager*) arg;
    COMXComponent* pComp = thData->GetOMXComp();

    TIMM_OSAL_U32 bufIndex = 0;
    COMXPortHandle* pCompPort;
    OMX_BUFFERHEADERTYPE *pBuffer, *pReFillBuffer;
    omxBufDoneCBType_t cbType;
	int nPort;

	MMS_IL_PRINT("STARTING OMX BufferDone CB thread on component %s : %d\n", pComp->compName, pComp->nComponentId);
    while (1)
	{
        MMS_IL_PRINT("Waiting OMX BufferDone CB on component : %d...\n", pComp->nComponentId);

		OMXbufferDone_t* pBuffDone = thData->Pop();
        if (thData->ThreadExit())
		{
             MMS_IL_PRINT("ThreadExit is true on comp %d\n", pComp->nComponentId);
            break;
        }

        if (pBuffDone == NULL)
		{
            MMS_IL_PRINT("Zero buffer pointer passed to BufferDone CB\n");
            continue;
        }

        cbType = pBuffDone->cbType;
        pBuffer = pBuffDone->pBuffer;

        MMS_IL_PRINT("OMX BufferDone CB received on comp %d\n", pComp->nComponentId);

        pReFillBuffer = pBuffer;

		nPort = (cbType == OMX_FILL) ? pBuffer->nOutputPortIndex : pBuffer->nInputPortIndex;

		pCompPort = pComp->GetPortData(nPort);
		if (pCompPort == NULL)
		{
			continue;
		}

        MMS_IL_PRINT("Running Port Services on comp %d\n", pComp->nComponentId);
		pComp->RunPortServices(pCompPort, pBuffer);

        if (pCompPort->tPortDef.bEnabled == OMX_FALSE)
        {
            MMS_IL_PRINT("OMX Component port is disabled - exiting with nothing to do\n");
            pComp->FreeOMXBuffer(pBuffer, pCompPort->tPortDef.nPortIndex);
            continue;
        }

        if (pCompPort->nDisplayId != -1 && pComp->displayModule != NULL && (pCompPort->tPortDef.eDomain == OMX_PortDomainVideo || pCompPort->tPortDef.eDomain == OMX_PortDomainImage))
        {
           // MMS_IL_PRINT("Displaying frame buffer\n");
            TIMM_OSAL_PTR frameBuffer;
            TIMM_OSAL_PTR pUV_Buf = NULL;
            TIMM_OSAL_U32 imgWidth = 0, imgHeight = 0, imgStride = 0;
            TIMM_OSAL_U32 offstX, offstY;

            if (pCompPort->tPortDef.eDomain == OMX_PortDomainVideo)
			{
                imgWidth = pCompPort->tPortDef.format.video.nFrameWidth;
                imgHeight = pCompPort->tPortDef.format.video.nFrameHeight;
                imgStride = pCompPort->tPortDef.format.video.nStride;
            }
            else if(pCompPort->tPortDef.eDomain == OMX_PortDomainImage)
			{
                imgWidth = pCompPort->tPortDef.format.image.nFrameWidth;
                imgHeight = pCompPort->tPortDef.format.image.nFrameHeight;
                imgStride = pCompPort->tPortDef.format.image.nStride;
                MMS_IL_PRINT("Calling DSS to display image buffer %p (%u x %u, %u) on comp %d\n", pBuffer->pBuffer, imgWidth, imgHeight, imgStride, pComp->nComponentId);
            }

            if (imgStride == 0)
			{
                imgStride = imgWidth;
            }

           // MMS_IL_PRINT("pBuffer->nOffset %lu\n", pBuffer->nOffset);
            offstX = pBuffer->nOffset % (imgStride);
            offstY = pBuffer->nOffset / (imgStride);

            MMS_IL_PRINT("Calling DSS to display frame buffer %p (%u x %u at %u, %u) on comp %d\n", pBuffer->pBuffer, imgWidth, imgHeight, offstX, offstY, pComp->nComponentId);

            if (pCompPort->eAllocType == MEM_ION_1D)
			{
				CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
               //  frameBuffer = pBuffer->pBuffer;
                frameBuffer = ionAlloc->getMappedAddr(pBuffer->pBuffer);
            } else if (pCompPort->eAllocType == MEM_ION_2D)
			{
				CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
                OMX_TI_BUFFERDESCRIPTOR_TYPE *Ion2DBuf = (OMX_TI_BUFFERDESCRIPTOR_TYPE *) (pBuffer->pBuffer);
                //frameBuffer = Ion2DBuf->pBuf[0];
                frameBuffer = ionAlloc->getMappedAddr(Ion2DBuf->pBuf[0]);
                if (Ion2DBuf->pBuf[1])
				{
                    //pUV_Buf =Ion2DBuf->pBuf[1];
                    pUV_Buf = ionAlloc->getMappedAddr(Ion2DBuf->pBuf[1]);
                    if (pUV_Buf == NULL)
					{
                        MMS_IL_PRINT("Getting UV buffer failed, colors will be distorted\n");
                    }
                }
            } else
			{
                frameBuffer = pBuffer->pBuffer;
                if (((OMX_TI_PLATFORMPRIVATE *)pBuffer->pPlatformPrivate)->pAuxBuf1)
				{
                    pUV_Buf = ((OMX_TI_PLATFORMPRIVATE *)pBuffer->pPlatformPrivate)->pAuxBuf1;
                }
            }//if (pCompPort->eAllocType == MEM_ION_1D)

            if (frameBuffer == NULL)
			{
                MMS_IL_PRINT("Zero buffer pointer, cannot display frame\n");
                continue;
            }

			frameBuffer = pComp->displayModule->DisplayFrame(frameBuffer, pUV_Buf, offstX, offstY, imgWidth, imgHeight);
             // MMS_IL_PRINT("DisplayFrame Passed!\n");
            if (frameBuffer == NULL)
			{
               // MMS_IL_PRINT("frameBuffer is NULL!!!\n");
                continue;
            }

            if (pCompPort->eAllocType == MEM_ION_1D || pCompPort->eAllocType == MEM_ION_2D)
			{
				CMemoryAllocator* ionAlloc = CMemoryAllocatorFactory::CreateMemoryAllocatorHandle(ION_MemoryAllocatorType);
               // MMS_IL_PRINT("Getting Buffer Handle!\n");
                frameBuffer = ionAlloc->getBufferFd(frameBuffer);
               // MMS_IL_PRINT("Getting Buffer Handle done!\n");
                if (frameBuffer == NULL)
				{
                    continue;
                }
            }

            MMS_IL_PRINT("DSS freed buffer %p, on comp %d\n", frameBuffer, pComp->nComponentId);

            pReFillBuffer = NULL;
            for (bufIndex = 0; bufIndex < pCompPort->nBufferCount; bufIndex++)
			{
                TIMM_OSAL_PTR pBuf;
                if (pCompPort->eAllocType == MEM_ION_2D)
				{
                    pBuf = ((OMX_TI_BUFFERDESCRIPTOR_TYPE *)(pCompPort->pComponentBuffers[bufIndex].pBuffer->pBuffer))->pBuf[0];
                }
				else
				{
                    pBuf = pCompPort->pComponentBuffers[bufIndex].pBuffer->pBuffer;
                }
                if (pBuf == frameBuffer)
				{
                    // Refill the last buffer that was freed by DSS
                    pReFillBuffer = pCompPort->pComponentBuffers[bufIndex].pBuffer;
                    break;
                }
            }
            if (pReFillBuffer == NULL)
			{
                MMS_IL_PRINT("Buffer %p not found on port %lu\n", frameBuffer, pCompPort->tPortDef.nPortIndex);
                continue;
            }
        } //else  MMS_IL_PRINT(" nDisplayId = %d, displayModule = 0x%08X, eDomain = %d, eDomain = %d\n",
            //pCompPort->nDisplayId, pComp->dispatcher->displayModule, tPortDef.eDomain, tPortDef.eDomain);

        if (!pCompPort->bAutoBufferCycle)
		{
            //sysGenInternalEvent(EVNT_OMX_PORT_BUFFER_EVENT,
            //                    (cbType == OMX_FILL) ?
            //                    EVNT_STATUS_PORT_FILLBUFFER_DONE :
            //                    EVNT_STATUS_PORT_EMPTYBUFFER_DONE, 0, 0);
            for (bufIndex = 0; bufIndex < pCompPort->nBufferCount; bufIndex++)
			{
                if (pCompPort->pComponentBuffers[bufIndex].pBuffer == pReFillBuffer)
				{
                    break;
                }
            }
        }

        if (pCompPort->pPortCallback)
		{
            MMS_IL_PRINT("Transferring buffer to linked OMX component\n");
            // If error is present we should give the buffer
            // back to the current component - otherwise it
            // will be lost
            if ((OMX_ERRORTYPE)pCompPort->pPortCallback(pCompPort, pReFillBuffer) != OMX_ErrorNone)
			{
                MMS_IL_PRINT("Reusing the buffer due to error in linked OMX component\n");
                if (pCompPort->bAutoBufferCycle)
				{
                    if (cbType == OMX_FILL)
					{
                        pComp->FillThisBuffer(pReFillBuffer);
                    }
					else
					{
                        pComp->EmptyThisBuffer(pReFillBuffer);
                    }
                }
				else
				{
                    pCompPort->pComponentBuffers[bufIndex].bFree = TIMM_OSAL_TRUE;
                }
            }
        }
		else
		{
            if (pCompPort->bAutoBufferCycle)
			{
                if (cbType == OMX_FILL)
				{
                    pComp->FillThisBuffer(pReFillBuffer);
                }
				else
				{
                    pComp->EmptyThisBuffer(pReFillBuffer);
                }
            }
			else
			{
                pCompPort->pComponentBuffers[bufIndex].bFree = TIMM_OSAL_TRUE;
            }

        }
	}

	MMS_IL_PRINT("EXITING OMX BufferDone CB thread on component %s : %d\n", pComp->compName, pComp->nComponentId);
    return NULL;
}