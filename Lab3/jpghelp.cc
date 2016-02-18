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


int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;

    if(argc < 3)
      {
        printf("\nUsage: server host-port dir\n");
        return 0;
      }
    else
      {
        nHostPort=atoi(argv[1]);
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
    
    printf("\nBinding to port %d",nHostPort);

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


    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }
	int optval = 1;
	setsockopt (hServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);

        printf("\nGot a connection from %X (%d)\n",
              Address.sin_addr.s_addr,
              ntohs(Address.sin_port));
        memset(pBuffer,0,sizeof(pBuffer));
        int rval = read(hSocket,pBuffer,BUFFER_SIZE);
		printf("Got from browser %d\n%s\n",rval, pBuffer);
		#define MAXPATH 1000
		char path[MAXPATH];
		rval = sscanf(pBuffer,"GET %s HTTP/1.1",path);
		printf("Got rval %d, path %s\n",rval,path);
		char fullpath[MAXPATH];
		sprintf(fullpath,"%s%s",argv[2], path);
		printf("fullpath %s\n",fullpath);
		memmove(&path[0], &path[0 + 1], strlen(path) - 0);
		printf("File %s\n",path);
		
		struct stat filestat;    
//		cout<< "THINGY: "<<stat(filename, &filestat)<<" yep";
        if(stat(path, &filestat)) {
                cout <<"File does not exist!\n";
				sprintf(pBuffer,"HTTP/1.1 404 Not found\r\n\r\n<!DOCTYPE html>\n<html>\n<head>\n<title>File Not Found</title>\n</head>\n<body><h1>FILE NOT FOUND!</h1>\n<p>Are you sure it exists?</p>\n</body>\n</html>");
				write(hSocket,pBuffer, strlen(pBuffer));
        }
		else{
			if(S_ISREG(filestat.st_mode)) {
				memset(pBuffer,0,sizeof(pBuffer));
				cout << path << " is a regular file \n";
		        cout << "file size = "<<filestat.st_size <<"\n";
		        FILE *fp = fopen(path,"r");
		        long lSize;
				fseek (fp , 0 , SEEK_END);
				lSize = ftell (fp);
				rewind (fp);
				char *buffer = (char *)malloc(sizeof(char)*lSize);
				fread(buffer, 1, lSize,fp);
				if(strstr(path,"html")){
						sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
							Content-Type: text/html\r\n\
							Content-Length: %d\
							\r\n\r\n", strlen(buffer));
				}
				else if(strstr(path,"txt")){
						sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
							Content-Type: text/plain\r\n\
							Content-Length: %d\
							\r\n\r\n", strlen(buffer));
				}
				else if(strstr(path,"gif") || strstr(path,"jpg")){
				sprintf(pBuffer,"HTTP/1.1 200 OK\r\n\
							Content-Type: image/jpg\r\n\
							Content-Length: %d\
							\r\n\r\n", sizeof(buffer));
				}	
				write(hSocket,pBuffer, strlen(pBuffer));
				/////////////
				struct stat filestat;    
		//		cout<< "THINGY: "<<stat(filename, &filestat)<<" yep";
				/*if(stat(path, &filestat)) {
				        cout <<"Index.html does not exist\n";
					
				}*/

		        
				write(hSocket,buffer,lSize);
				free(buffer);
				fclose(fp);
			}
       		else{
       			cout << path << " is a DIRECTORY\n";
       		 	string thing = "";
				//path + "/index.html";
				char array2[] = "/index.html";
				char * newArray = new char[strlen(path)+strlen(array2)+1];
				strcpy(newArray,path);
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
						Content-Length: %d\
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

					dirp = opendir(path);			
					
					thing.append(std::string("<!DOCTYPE html>\n")
									+ "<html>\n"
									+ "<head>\n"
									+ "<title>"
									+ path + "</title>\n"
									+ "</head>\n"
									+ "<body><p>" + path + "</p>\n");
					while ((dp = readdir(dirp)) != NULL){
						//printf("name %s\n", dp->d_name);
						if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0){
							/*cout << "Skipping " << dp->d_name << "\n";
							
							int value = strlen(((patch::to_string(Address.sin_addr.s_addr)).c_str())) 
										+ strlen((patch::to_string(ntohs(Address.sin_port))).c_str());                        
							char *buffer2 = (char *)malloc(value);
							sprintf(buffer2, "%d:%d",Address.sin_addr.s_addr,ntohs(Address.sin_port));
							cout << "Host: " << buffer2 << "\n";
							
							string host= "";
							host.append(std::string("http://") 
											+ (std::to_string(Address.sin_addr.s_addr)).c_str()
											+ ":" + ntohs(Address.sin_port));
							if(strcmp(dp->d_name,"..") == 0 && strcmp(buffer2,path) != 0){
								//thing.append(std::string("<p><a href=\"/") + path + "/" + dp->d_name + "\">" + "BACK" + "</a></p>\n");
							}*/
						}
						else {
							thing.append(std::string("<p><a href=\"/") + path + "/" + dp->d_name + "\">" + dp->d_name + "</a></p>\n");
						}
					}	
					(void)closedir(dirp);
					thing.append(std::string("</body>\n")
							+ "</html>\n");
					//cout << "Default index:\n" << thing << "\n";

					/////
					char *buffer = (char *)malloc((thing.size()));
					char yBuffer[10000];
					sprintf(buffer, "%s", thing.c_str());
					sprintf(yBuffer,"HTTP/1.1 200 OK\r\n\
						Content-Type: text/html\r\n\
						Content-Length: %d\
						\r\n\r\n", strlen(buffer));
					write(hSocket,yBuffer,strlen(yBuffer));
               		printf("Sending\n%s",buffer);
					//strcpy (buffer, thing.c_str());
					//fread(buffer, 1, sizeof(thing.c_str()),thing.c_str());
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

