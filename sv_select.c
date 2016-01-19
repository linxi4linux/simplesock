#include  <stdio.h>   
#include  <sys/socket.h>   
#include  <unistd.h>   
#include  <sys/types.h>   
#include  <netinet/in.h>   
#include  <stdlib.h>   
#include  <time.h>   
#include  <sys/select.h>
#include <errno.h>

#define MAX_BUFFER_SIZE 255   
int threadnum =0;
void useage(char * servername)
{
	printf("输入格式错误，正确格式为：%s  port\n",servername);
}


int action_after_recv(char * order, int port);

int main(int argc, char** argv)  
{  
	struct  sockaddr_in servaddr,cliaddr;  
	fd_set readset, tempset;
	int maxfd, flags;
	int srvsock, peersock, j, result, result1, sent, len;
	struct timeval tv;
	char buffer[MAX_BUFFER_SIZE+1];

	if(argc <2)
	{
		useage(argv[0]);
		exit(1);
	}
	if  ((srvsock  =  socket(AF_INET,SOCK_STREAM, 0 ))  <   0 )  
	{  
		printf( " create socket error!\n " );  
		exit( 1 );  
	}
	/* if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
	  perror("setsockopt");
	  exit(1);
	}   
	*/ 
	bzero( & servaddr, sizeof (servaddr));  

	servaddr.sin_family  =  AF_INET;  
	servaddr.sin_port  =  htons(atoi(argv[1]));  
	servaddr.sin_addr.s_addr  =  htons(INADDR_ANY);  

	if  (bind(srvsock,( struct  sockaddr * ) & servaddr, sizeof (servaddr)) < 0 )  
	{  
		printf( " bind to port %d failure!\n " ,servaddr.sin_port);  
		exit( 1 );  
	}   
	int sock_lis = listen(srvsock,0);

	if  (sock_lis   <   0 )  
	{  
		printf( " call listen failure!\n " );  
		exit( 1 );  
	}   
	FD_ZERO(&readset);
	FD_SET(srvsock, &readset);
	maxfd = srvsock;

	do {
		memcpy(&tempset, &readset, sizeof(tempset));
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

		if (result == 0) {
			printf("\nselect() timed out!\n");
		}
		else if (result < 0 && errno != EINTR) {
			printf("Error in select(): %s\n", strerror(errno));
		}
		else if (result > 0) {

			if (FD_ISSET(srvsock, &tempset)) {
				len = sizeof(cliaddr);
				peersock = accept(srvsock,(struct sockaddr *) &cliaddr, &len);
				if (peersock < 0) {
					printf("Error in accept(): %s\n", strerror(errno));
				}
				else {
					threadnum++;
					printf("%d connections are built!\n",threadnum);
					FD_SET(peersock, &readset);
					maxfd = (maxfd < peersock)?peersock:maxfd;
				}
				FD_CLR(srvsock, &tempset);
			}

			for (j=0; j<maxfd+1; j++) {
				if (FD_ISSET(j, &tempset)) {

					do {
						bzero(buffer,sizeof(buffer));
						result = recv(j, buffer, MAX_BUFFER_SIZE, 0);
					} while (result == -1 && errno == EINTR);

					if (result > 0) {
					//	buffer[result] = 0;
					//	printf("Echoing: %s\n", buffer);
						printf("%s", buffer);
						sent = 0;

						do {
							result1 = send(j, buffer+sent, result-sent, MSG_NOSIGNAL);
							if (result1 > 0)
								sent += result1;
							else if (result1 < 0 && errno != EINTR);
							break;
						} while (result > sent);

					}
					else if (result == 0) {
						close(j);
						FD_CLR(j, &readset);
						threadnum--;
					}
					else {
						printf("Error in recv(): %s\n", strerror(errno));
					}
				}      // end if (FD_ISSET(j, &tempset))
			}      // end for (j=0;...)
		}      // end else if (result > 0)
	} while (1);
	return 0;
}


int action_after_recv(char * order,int port)
{
	printf("recv from client(%d):: %s\n",port,order);
	return 0;
}
