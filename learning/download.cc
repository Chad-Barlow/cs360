#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255
using std::string;

int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
	int option;
	bool debugging;
	int count;
	string url = "localhost";

    if(argc < 4)
      {
        printf("\nUsage: client host-name host-port URL options: -c count, -d debugging\n");
        return 0;
      }
    else
      {
		while ((option = getopt(argc,argv,"d:c:")) != -1) {
			switch (option) {
				case 'd':
					debugging = true;
					//port = atoi(optarg);
					break;
				case 'c':
					count = atoi(optarg);
					//host = optarg;
					break;
				default:
					//cout << "client [-d debugging] [-c count]" << endl;
					exit(EXIT_FAILURE);
			}
		}
  
        strcpy(strHostName,argv[1]);
        nHostPort=atoi(argv[2]);
		URL = argv[3];
      }

    printf("\nMaking a socket");
    /* make a socket */
    hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    /* connect to host */
    if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
    #define MAXGET 1000
	//figure out how to do memory management in c right here.
	char *message = (char *)malloc(MAXGET);
	sprintf(message, "GET /foo.html HTTP/1.1\r\nHost:mclement.us:80\r\n\r\n");
	// send HTTP on the socket
	printf("Request: %s\n", message);
	write(hSocket,message,strlen(message));
	//Read response back from the socket
    nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
	printf("Response: %s\n", pBuffer);

    printf("\nClosing socket\n");
    /* close socket */                       
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
	free(message);
}