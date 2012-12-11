#include "inc/Communication/Media/Tcp/CCommunicationMediaTcp.h"

extern char tcp_port[5];

CCommunicationMediaTcp::~CCommunicationMediaTcp()
{

}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::TCPIP_ReceiveNumBytes(void* pBuffer, unsigned int nNumBytes)
{
    unsigned int bytesread = 0;
    struct pollfd fds;
    int ret = 0;

    fds.fd = fd;
    fds.events = POLLIN;
   // printf("TCPIP_ReceiveNumBytes()-- before poll()\n\n");
    //wait infinite timeout
    ret = poll(&fds, 1, -1);
    
    if (ret <= 0)
    {
        //printf("error - poll returned -1\n\n"); // error occurred in poll()
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    else 
    {
        // check for events on s1:
        //     if (fds.revents & POLLIN) {
        //
        ////MMS_IL_PRINT("TCPIP_ReceiveNumBytes()-- before read()\n\n");
        bytesread = recv(fd, pBuffer, nNumBytes, 0);

        ////MMS_IL_PRINT("bytes read %d, numbytes:%d:\nReceived: ", bytesread, nNumBytes);
        //for(i=0; i<bytesread; ++i) {
            //MMS_IL_PRINT("0x%X ", ((char*)pBuffer)[i]);
        //  }
        //MMS_IL_PRINT("\n");

        if ( bytesread != nNumBytes ) 
        {
            printf("bytes read do not match with requested bytes\n\n");
            return TIMM_OSAL_ERR_MSG_SIZE_MISMATCH;
        }
    }
    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::TCPIP_WriteNumBytes(void* pBuffer, unsigned int nNumBytes)
{
    unsigned int byteswritten = 0;

    byteswritten = send(fd, pBuffer, nNumBytes, 0);
    //MMS_IL_PRINT("bytes written %d, numbytes:%d:\n", byteswritten, nNumBytes);
    if ( byteswritten != nNumBytes )
    {
        //MMS_IL_PRINT("bytes written != nNumBytes\n\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}

static void sigchld_handler(int s)
{   //wait for any child process to stop
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
static void* get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::TCPIP_Messaging_Init_Function()
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    printf("OMAP Server IL starting server\n");

    if ((rv = getaddrinfo(NULL, tcp_port, &hints, &servinfo)) != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
		//init socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            printf("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            printf("setsockopt FAILED\n");
            return TIMM_OSAL_ERR_UNKNOWN;
        }
		//Bind socket to address
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            printf("server: bind error msg: %s\n", strerror(errno));
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        printf("server: failed to bind\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    freeaddrinfo(servinfo); // all done with this structure

	//Listen for incoming connection
    if (listen(sockfd, BACKLOG) == -1)
    {
        printf("listen failed");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	//Get handler for sigchld_handler function
    sa.sa_handler = sigchld_handler; // reap all dead processes

	//Initialise and empty a signal set
    sigemptyset(&sa.sa_mask);
	//Don't fail interrupeable functions, interruped by thi signal
    sa.sa_flags = SA_RESTART;
	//Examine and change signal action

	//	SIGCHLD Child process terminated or stopped.
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        printf("sigaction failed");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    sin_size = sizeof their_addr;
    printf("OMAP Server IL awaiting connection...\n");
	//Accept conncetcion
    fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (fd == -1)
    {
        printf("accept connection failed");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	//Convert IP address to string
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("OMAP Server IL connected\n");

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::TCPIP_Messaging_Deinit_Function()
{
    int error = TIMM_OSAL_ERR_NONE;

    error += shutdown(fd,SHUT_RDWR);
    error += shutdown(sockfd,SHUT_RDWR);
	//Close sockets
    error += close(fd);
//    printf("Closing Socket fd returned %d\n", error);
    error += close(sockfd);
//    printf("Closing Socket sockfd returned %d\n", error);
    if (error == TIMM_OSAL_ERR_NONE)
        printf("Main process: Closing Sockets!\n");

     return error;
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::Init()
{
	//Init mutex semaphore with 1
	sem_init(&mutex, 0, 1);
	//Init TCP connection
   return TCPIP_Messaging_Init_Function();
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::Deinit()
{
    int error = TIMM_OSAL_ERR_NONE;
	//Close TCP connection
    error = TCPIP_Messaging_Deinit_Function();
	//Destroy Semaphore mutex
	sem_destroy(&mutex);

    return error;
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::Receive(void* pCurMessage)
{
    systemMessage_t* message = (systemMessage_t*)pCurMessage;
    unsigned int nMessageSize = 0;

    message->pPayload = NULL;

    if (!message)
    { 
        MMS_IL_PRINT("Allocation failed failed! Error - 0x%08X\n", TIMM_OSAL_ERR_ALLOC);
        return TIMM_OSAL_ERR_ALLOC;
    }

    /* Receive message header */
    if (TIMM_OSAL_ERR_NONE != TCPIP_ReceiveNumBytes(message, sizeof(systemMessage_t) - sizeof(void*)))
    {
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    nMessageSize = ((systemMessage_t *)message)->nPayloadSize;
    //MMS_IL_PRINT("Payload data size = %d\n", nMessageSize);

    if(nMessageSize > 0)
    {
        if (nMessageSize > MSG_MAX_PAYLOAD)
        {
            //MMS_IL_PRINT("Invalid message size received!!! 0x%08d", nMessageSize);
            return TIMM_OSAL_ERR_MSG_SIZE_MISMATCH;
        }

        /* Allocate space for message payload */
        message->pPayload = malloc(nMessageSize);

        if (!message->pPayload)
        {
            //MMS_IL_PRINT("Inssuficient memory to allocate message payload!");
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        /* Receive message payload */
        if (0 != TCPIP_ReceiveNumBytes(message->pPayload, message->nPayloadSize))
        {
            return TIMM_OSAL_ERR_UNKNOWN;
        }
    }//if(nMessageSize > 0)

    //MMS_IL_PRINT("Transfering message with id %d and payload of %d to handles-task", message->nId, message->nPayloadSize);

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CCommunicationMediaTcp::Send(void* pMessage)
{
	sem_wait(&mutex);

    systemMessage_t* message = (systemMessage_t*)pMessage;
    //Send message over TCP/IP
    if (TIMM_OSAL_ERR_NONE != TCPIP_WriteNumBytes(message, sizeof(systemMessage_t) - sizeof(void*)))
        return TIMM_OSAL_ERR_UNKNOWN;

   if (TIMM_OSAL_ERR_NONE !=  TCPIP_WriteNumBytes(message->pPayload, message->nPayloadSize))
         return TIMM_OSAL_ERR_UNKNOWN;

	sem_post(&mutex);

    return TIMM_OSAL_ERR_NONE;
}