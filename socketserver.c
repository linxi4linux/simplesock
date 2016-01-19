#include  <stdio.h>   
#include  <sys/socket.h>   
#include  <unistd.h>   
#include  <sys/types.h>   
#include  <netinet/in.h>   
#include  <stdlib.h>   
#include  <time.h>   
   
#define  BUFFER_SIZE 255   
//#define  LENGTH_OF_LISTEN_QUEUE  5 
void useage(char * servername)
{
	printf("输入格式错误，正确格式为：%s  port\n",servername);
}


int action_after_recv(char * order, int port);
int main(int argc, char** argv)  
{  
       int  servfd,clifd;  
       struct  sockaddr_in servaddr,cliaddr;  
  
			 if(argc <2)
			 {
				 useage(argv[0]);
				 exit(1);
			 }
       if  ((servfd  =  socket(AF_INET,SOCK_STREAM, 0 ))  <   0 )  
         {  
              printf( " create socket error!\n " );  
              exit( 1 );  
       }   
   
       bzero( & servaddr, sizeof (servaddr));  
  
       servaddr.sin_family  =  AF_INET;  
       servaddr.sin_port  =  htons(atoi(argv[1]));  
       servaddr.sin_addr.s_addr  =  htons(INADDR_ANY);  
  
       if  (bind(servfd,( struct  sockaddr * ) & servaddr, sizeof (servaddr)) < 0 )  
         {  
              printf( " bind to port %d failure!\n " ,servaddr.sin_port);  
              exit( 1 );  
       }   
   
        if  (listen(servfd,/*LENGTH_OF_LISTEN_QUEUE*/ 0)  <   0 )  
         {  
              printf( " call listen failure!\n " );  
              exit( 1 );  
       }   
   
        while  ( 1 )  
         { // server loop will nerver exit unless any body kill the process   
  
              char  buf[BUFFER_SIZE];  
              long  timestamp;  
              socklen_t length  =   sizeof (cliaddr);  
              clifd  =  accept(servfd,( struct  sockaddr * ) & cliaddr, & length);  
  
               if  (clifd  <   0 )  
                {  
                     printf( " error comes when call accept!\n " );  
                     break ;  
              }   
   
              printf( " from client,IP:%s,Port:%d\n " ,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));  
							int pid=fork();
							if(pid <0)
							{
								printf("fork error! exit!\n");
								exit(1);
							}
							else if(pid == 0 )   //子进程
              {
								close(servfd);
							while(1)
							{
              bzero(buf,BUFFER_SIZE);
							length = recv(clifd,buf,BUFFER_SIZE,0);
							if(length < 0)
							{
								printf("recv error!\n");
								return -1;
							}
							else if(length ==0)
							{
                 
							}
							else
							{
                action_after_recv(buf,ntohs(cliaddr.sin_port));
						//		sleep(1);
                send(clifd,buf,BUFFER_SIZE, 0 );  
							  printf("sent to client::%s\n",buf);
							}
							}
							close(clifd);
							return 0;
							}
							else   //父进程
							{
				        
							continue;
							}
  
       } // exit   
   
       close(servfd);  
  
        return   0 ;  
}


int action_after_recv(char * order,int port)
{
	printf("recv from client(%d):: %s\n",port,order);
	return 0;
}
