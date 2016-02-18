#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

main(int argc, char **argv)
{
        struct stat filestat;

        if(stat(argv[1], &filestat)) {
                cout <<"ERROR in stat\n";
        }
        if(S_ISREG(filestat.st_mode)) {
                cout << argv[1] << " is a regular file \n";
                cout << "file size = "<<filestat.st_size <<"\n";
                FILE *fp = fopen(argv[1],"r");
                char *buffer = (char *)malloc(filestat.st_size+1);
                //char *buffer = new Char*[filestat.st_size+1];
                fread(buffer, filestat.st_size,1,fp);
                //just write to the socket here in the real one
                printf("Got\n%s",buffer);
                free(buffer);
                fclose(fp);
        }
        if(S_ISDIR(filestat.st_mode)) {
        	cout << argv[1] << " is a directory \n";

		    DIR *dirp;
		    struct dirent *dp;

		    dirp = opendir(".");
		    while ((dp = readdir(dirp)) != NULL){
		          printf("name %s\n", dp->d_name);
		    }
		    (void)closedir(dirp);
        }
}

