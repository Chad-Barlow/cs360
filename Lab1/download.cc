#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         500
#define HOST_NAME_SIZE      255
using std::string;
using namespace std;

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
	char option;
	int debugging = 0;
	int count =1;
	string url = "localhost";
	extern char *optarg;
	bool check = false;
	
	//Handling the arguments
    if(argc < 4){
        printf("\nUsage: client host-name host-port URL options: -c count, -d debugging\n");
        return 0;
    }
    else{
		while ((option = getopt(argc,argv,"c:d")) != -1) {
			switch (option) {
				case 'd':
					cout << "DEBUG MODE";
					debugging = 1;
					break;
				case 'c':
					count = atoi(optarg);
					check = true;
					break;
				default:
					cout << "client [-d debugging] [-c count]" << endl;
					exit(EXIT_FAILURE);
			}
		}
    }
    
	if (count <= 0){
		perror("Invalid count. Must be 1 or more.");
		return 0;
	}
    
    strcpy(strHostName,argv[optind]);
    nHostPort=atoi(argv[optind+1]);
    int i=0;
	char * str = argv[optind+1];
	while (str[i]){
		if (isalpha(str[i])){
			printf ("Value %s is malformed, please enter an integer.\n",str);
			return 0;
		}
		i++;
	}
	url = argv[optind+2];
    
    /* make a socket */
    //printf("\nMaking a socket");
    hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    if(pHostInfo==NULL){
    	cout << "Failed to get Host Info. Bad Host\n";
    	return 0;
    }
    
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;
   
    /* connect to host */
    //printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);
    if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
    
    #define MAXGET 1000
	//figure out how to do memory management in c right here.
	char *message = (char *)malloc(MAXGET);
	char c_url[1024];
	strcpy(c_url, url.c_str());
	sprintf(message, "GET %s HTTP/1.0\r\nHost:%s:%d\r\n\r\n",c_url,strHostName,nHostPort);
	//sprintf(message, "GET /foo.html HTTP/1.1\r\nHost:mclement.us:80\r\n\r\n");
	
	if(debugging){
		printf("\nRequest:\n%s\n", message);
	}
	
	string header = "";
	int successes = 0;
	for(int i = 0; i<count; i++){
		
		// send HTTP on the socket
		write(hSocket,message,strlen(message));

		//Read response back from the socket
		while (header.find("\r\n\r\n") == string::npos) {
		    int nread = recv(hSocket,pBuffer,100,0);
		    if (nread < 0) {
		        if (errno == EINTR){
		           perror("the socket call was interrupted -- try again\n");
		           return 0;
		        }
		        else {
		            // an error occurred, so break out
		            perror("An error occured reading the socket\n");
		            return 0;
		        }
		    } else if (nread == 0) {
		        // the socket is closed
		        break;
		    }
		    // be sure to use append in case we have binary data
		    header.append(pBuffer,nread);
		}
		//cout << "Header: \n" << header << "\n";
		if(check){
			if(header.find("200 OK") !=-1){
				successes+=1;
			}	
		}
	}
	
	if(check){
		cout << "Downloaded succesfully " << successes << "/" << count << " times.\n";
		return 0;
	}
	
	if(header.find("404 Not Found") != -1){
		cout << "Page not found - Bad endpoint\n";
		return 0;
	}

    //Grab the body length
    int start = header.find("Content-Length");
    int end = header.find("Content-Type");
    int len_ = end-1-(start+14);
    string len = header.substr(start+15,len_);
    //cout << "Content-len: " << len << "\n";
    
    //Grab the message and clean up the header
    int s1 = header.find("\r\n\r\n");
    string response = header.substr(s1+1);
    header = header.substr(0,header.length() - response.length());
    if(debugging){
	    cout << "Header: \n" << header << "\n\n";
	}    
    int needed = atoi(len.c_str()) - response.length();
    int readg=0;
    while(readg < needed){
	    int readn = recv(hSocket,pBuffer,100,0);
	    readg += readn;
		response.append(pBuffer,readn);
	}
    
    /*
	nReadAmount = 1;
	string response =""; 
	while(nReadAmount != 0){
	    nReadAmount = read(hSocket,pBuffer,100); //run this multiple times to get full response. only want to display the actual message. get the content length from the header.
		//response += pBuffer;
		response.append(pBuffer,nReadAmount);
		cout << "\nAdding: " << pBuffer << "\n";
		cout << "\n" << nReadAmount;
		//printf("\nAdding: %s", pBuffer);
		if(nReadAmount == -1){
			cout<<"Error reading socket";
			break;
		}	    
	}*/
	
	if(debugging){
		cout << "Response:\n" << response << "\n";
	}	
	else{
		cout << response << "\n";
	}

    /* close socket */                       
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
	free(message);
}
