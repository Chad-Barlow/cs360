#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <queue>
#include <pthread.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <semaphore.h>
using namespace std;

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

class guber
{
	public:
	int push(int qel) {
		if(q.size() < 20){
			q.push(qel);
			return 1;
		}
		else{
			//queue is full
			return -1;
		}
	};
	int pop() {
		int tmp;
		tmp = q.front();
		q.pop();
		return(tmp);
	};
	int getSize(){
		return q.size();
	}
	private:
	queue <int> q;
};

guber connectionQ;
bool debugging;
bool printReq;
bool printRes;
int  counter = 0;
sem_t sem;

struct package{
	char hostport[BUFFER_SIZE];
   	char pBuffer[BUFFER_SIZE];
   	char* argv;
};

void handler (int status);
void handler (int status)
{
	printf("received signal %d\n",status);
}

void *runThread(void *p){
	struct package *pack;
	pack = (struct package *) p;
	char hostport[BUFFER_SIZE];
	strncpy(hostport, pack->hostport, sizeof(pack->hostport));
	char pBuffer[BUFFER_SIZE];
	strncpy(pBuffer, pack->pBuffer, sizeof(pack->pBuffer));
	char* argv = pack->argv;
	int tmp;
	
	sem_wait(&sem);
    tmp = counter;
    //sleep(1);
    tmp++;
    counter = tmp;
    printf("Counter value: %d\n",counter);
    
	for(;;){	
		sleep(1);
		//Dequeue a socket if one is ready
		if(connectionQ.getSize()>0){
		
			int hSocket = connectionQ.pop();
			printf("\nUsing socket %i\n",hSocket);
			//Process socket
			printf("\nGot a connection from %c\n",*hostport);
			memset(pBuffer,0,sizeof(pBuffer));
			int rval = read(hSocket,pBuffer,BUFFER_SIZE);
			printf("Got from browser %d\n%s\n",rval, pBuffer);
			#define MAXPATH 1000
			char path[MAXPATH];
			rval = sscanf(pBuffer,"GET %s HTTP/1.1",path);
			printf("Got rval %d, path %s\n",rval,path);
			char fullpath[MAXPATH];
			sprintf(fullpath,"%s%s",argv, path);
			printf("fullpath %s\n",fullpath);
			memmove(&path[0], &path[0 + 1], strlen(path) - 0);
			printf("File %s\n",path);

			struct stat filestat;    

			if(stat(fullpath, &filestat)) {
					cout <<"File does not exist!\n";
					sprintf(pBuffer,"HTTP/1.1 404 Not found\r\n\r\n<!DOCTYPE html>\n<html>\n<head>\n<title>File Not Found</title>\n</head>\n<body><h1>FILE NOT FOUND!</h1>\n<p>Are you sure it exists?</p>\n</body>\n</html>");
					write(hSocket,pBuffer, strlen(pBuffer));
			}
			else{
				if(S_ISREG(filestat.st_mode)) {
					memset(pBuffer,0,sizeof(pBuffer));
					cout << path << " is a regular file \n";
					cout << "file size = "<<filestat.st_size <<"\n";
					FILE *fp = fopen(fullpath,"r");
					long lSize;
					fseek (fp , 0 , SEEK_END);
					lSize = ftell (fp);
					rewind (fp);
					char *buffer = (char *)malloc(sizeof(char)*lSize);
					fread(buffer, 1, lSize,fp);
					if(strstr(path,"html")){
							sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
								Content-Type: text/html\r\n\
								Content-Length: %lu\
								\r\n\r\n", strlen(buffer));
					}
					else if(strstr(path,"txt")){
							sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
								Content-Type: text/plain\r\n\
								Content-Length: %lu\
								\r\n\r\n", strlen(buffer));
					}
					else if(strstr(path,"gif")){
					sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
								Content-Type: image/gif\r\n\
								Content-Length: %lu\
								\r\n\r\n", sizeof(buffer));
					}
					else if(strstr(path,"jpg")){
					sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
								Content-Type: image/jpg\r\n\
								Content-Length: %lu\
								\r\n\r\n", sizeof(buffer));
					}		
					write(hSocket,pBuffer, strlen(pBuffer));

					struct stat filestat;    
				
					write(hSocket,buffer,lSize);
					free(buffer);
					fclose(fp);
				}
				else{
					cout << path << " is a DIRECTORY\n";
				 	string thing = "";
					//path + "/index.html";
					char array2[] = "/index.html";
					char * newArray = new char[strlen(fullpath)+strlen(array2)+1];
					strcpy(newArray,fullpath);
					strcat(newArray,array2);
					if(stat(newArray, &filestat) == 0){	
						FILE *fp = fopen(newArray,"r");
						long lSize;
						fseek (fp , 0 , SEEK_END);
						lSize = ftell (fp);
						rewind (fp);
						char *buffer = (char *)malloc(sizeof(char)*lSize);
						fread(buffer, 1, lSize,fp);
						cout << "Servering index.html\n";
						memset(pBuffer,0,sizeof(pBuffer));
						sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
							Content-Type: text/html\r\n\
							Content-Length: %lu\
							\r\n\r\n", strlen(buffer));
						write(hSocket,pBuffer, strlen(pBuffer));
						write(hSocket,buffer,lSize);
						free(buffer);
						fclose(fp);
			
					}
					else{
			
						int len;
						DIR *dirp;
						struct dirent *dp;

						dirp = opendir(fullpath);			
			
						thing.append(std::string("<!DOCTYPE html>\n")
										+ "<html>\n"
										+ "<head>\n"
										+ "<title>"
										+ path + "</title>\n"
										+ "</head>\n"
										+ "<body><p>" + path + "</p>\n");
						while ((dp = readdir(dirp)) != NULL){
							//printf("name %s\n", dp->d_name);
							if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0){}
							else {
								if(strcmp(path,"") == 0){
									cout << "The path is: " << path << "\n";
									char temp[sizeof(fullpath)];

									thing.append(std::string("<p><a href=\"") + hostport + dp->d_name + "\">" + dp->d_name + "</a></p>\n");
								}
								else {
									thing.append(std::string("<p><a href=\"/") + path + "/" + dp->d_name + "\">" + dp->d_name + "</a></p>\n");
								}
							}
						}	
						(void)closedir(dirp);
						thing.append(std::string("</body>\n")
								+ "</html>\n");

						char *buffer = (char *)malloc((thing.size()));
						char yBuffer[10000];
						sprintf(buffer, "%s", thing.c_str());
						sprintf(yBuffer,"HTTP/1.1 200 OK\r\n\
							Content-Type: text/html\r\n\
							Content-Length: %lu\
							\r\n\r\n", strlen(buffer));
						write(hSocket,yBuffer,strlen(yBuffer));
				   		printf("Sending\n%s",buffer);

						write(hSocket,buffer,strlen(buffer));
						free(buffer);
					}
				}
			}

			#ifdef notdef
			linger lin;
			unsigned int y=sizeof(lin);
			lin.l_onoff=1;
			lin.l_linger=10;
			setsockopt(hSocket,SOL_SOCKET, SO_LINGER,&lin,sizeof(lin));	
			shutdown(hSocket, SHUT_RDWR);
			#endif
			printf("\nClosing the socket");
			/* close socket */
			if(close(hSocket) == SOCKET_ERROR)
			{
				printf("\nCould not close socket\n");
				return 0;
			}
		}
	}
	sem_post(&sem);
}

int main(int argc, char* argv[])
{
	debugging = true;
	printReq = false;
	printRes = false;
    int option;
    int num_threads = 1;
    
	while ((option = getopt(argc,argv,"dUe")) != -1) {
	    switch (option) {
	        case 'd':
	            //option supresses the content from being output
	            debugging = false;
	            break;
			case 'U':
				//option prints request headers
				printReq = true;
				break;
			case 'e':
				//option prints response headers
				printRes = true;
				break;
	        default:
	            cout << "USAGE: port num_threads dir [-d]/[-U]/[-e]" << endl;
	            exit(EXIT_FAILURE);
	    }
	}

    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;

    if(argc < 4){
        printf("\nUsage: server host-port num_threads dir\n");
        return 0;
    }
    else if(atoi(argv[2]) <1){
    	printf("\nUsage: threads must be greater than 0\n");
    	return 0;
    }
    else{
        nHostPort=atoi(argv[1]);
        num_threads = atoi(argv[2]);
        printf("\nNumThreads: %d\n", num_threads);
    }

    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);
    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;
    
    printf("\nBinding to port %d\n",nHostPort);
    /* bind to a port */
    if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
                        == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
    
 	/*  get port number */
    getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
    printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

        printf("Server\n\
              sin_family        = %d\n\
              sin_addr.s_addr   = %d\n\
              sin_port          = %d\n"
              , Address.sin_family
              , Address.sin_addr.s_addr
              , ntohs(Address.sin_port)
            );

	char hostport[BUFFER_SIZE];
	sprintf(hostport,"http://%d:%d/", Address.sin_addr.s_addr, ntohs(Address.sin_port));
    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }
	int optval = 1;
	setsockopt (hServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));



	//semephore handling
	   sem_init(&sem, PTHREAD_PROCESS_PRIVATE, 1);
	
	// First set up the signal handler
	struct sigaction sigold, signew;

	signew.sa_handler=handler;
	sigemptyset(&signew.sa_mask);
	sigaddset(&signew.sa_mask,SIGINT);
	signew.sa_flags = SA_RESTART;
	sigaction(SIGINT,&signew,&sigold);

	//Create Threads
	for(int i=0;i<num_threads;i++){
		int rc;
		pthread_t thread;
		package p; 
		strncpy(p.hostport, hostport, sizeof(hostport));
		strncpy(p.pBuffer, pBuffer, sizeof(pBuffer));
		p.argv = argv[3];
		rc = pthread_create(&thread, NULL, runThread, &p);
		//pthread_join(thread, NULL);
	}
	
    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /*get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);

		printf("\nStoring socket %i\n",hSocket);
		//Queue the socket
		if(connectionQ.getSize()<20){
			connectionQ.push(hSocket);
		}
		else{
			//Queue is full
			printf("\nCould not add socket, queue is full\n");
		}
    }
}

