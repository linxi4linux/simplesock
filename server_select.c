#include  <stdio.h>   
#include  <sys/socket.h>   
#include  <unistd.h>   
#include  <sys/types.h>   
#include  <netinet/in.h>   
#include  <stdlib.h>   
#include  <time.h>   
#include  <sys/select.h>
#include <errno.h>
   
#define  BUFFER_SIZE 255   
#define  SETNUM 1000 

int con_num;             //连接数
int fd_ary[SETNUM];  

void useage(char * servername)
{
	printf("输入格式错误，正确格式为：%s  port\n",servername);
}


int action_after_recv(char * order, int port);
int main(int argc, char** argv)  
{  
       int  servfd;  
			 int new_f =0;
			 int yes=1;
			 char buf[BUFFER_SIZE];
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
			 if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {

        perror("setsockopt");

        exit(1);

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
			 int sock_lis = listen(servfd,0);
   
        if  (sock_lis   <   0 )  
         {  
              printf( " call listen failure!\n " );  
              exit( 1 );  
       }   

				fd_set f_read;
			//	fd_set f_write;
				int max_con;
				struct timeval tv;
				int i;

				con_num = 0;
				max_con =  servfd;

					for(i =0;i<SETNUM;i++)
					{
						fd_ary[i]= 0; 
          }   

				while(1)
				{
					FD_ZERO(&f_read);
					FD_SET(servfd, &f_read);
					tv.tv_sec =30;
					tv.tv_usec =0;

					for(i =0;i<SETNUM;i++)
					{
						if(fd_ary[i] != 0)
						{
							//printf("fd_ary[i]:%d\n",fd_ary[i]);
							FD_SET(fd_ary[i],&f_read);
						}
					}

					int ret = select(max_con+1,&f_read,NULL,NULL,&tv);
					if(ret < 0 && errno !=EINTR)
					{
						perror("select");
				//		break;
					}
					else if(ret ==  0)
					{
						printf("select() timed out!\n");
						continue;
					}
					else if(ret >0){
					if(FD_ISSET(servfd,&f_read))
					{
              socklen_t length  =   sizeof (cliaddr);  
              new_f  =  accept(servfd,( struct  sockaddr * ) & cliaddr, & length);  
							if(new_f <0)
							{
								perror("accept");
								//continue;
							}
							else if(new_f >0)     //一定要这样写，不然下面的new_f依然会添加到fd_ary中
							{
            printf( " from client,IP:%s,Port:%d\n " ,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));  
						FD_CLR(servfd,&f_read);
            
						if(con_num <SETNUM)
					{
						for(i=0;i<SETNUM;i++)
						{
							if(fd_ary[i]== 0)
							{
								fd_ary[i] =new_f;
								break;
							}
						}
						con_num++;
						if(new_f > max_con)
						{
							max_con = new_f;
						}
					}
					else
					{
             printf("max connections arrive, exit\n");
             send(new_f, "bye", 4, 0);
             close(new_f);
             break;
					}
							}

					}
					
				/*	if(con_num <SETNUM)
					{
						for(i=0;i<SETNUM;i++)
						{
							if(fd_ary[i]== 0)
							{
								fd_ary[i] =new_f;
								break;
							}
						}
						con_num++;
						if(new_f > max_con)
						{
							max_con = new_f;
						}
					}
					else
					{
             printf("max connections arrive, exit\n");
             send(new_f, "bye", 4, 0);
             close(new_f);
             break;
					}
*/
          for(i =0;i <SETNUM;i++)
					{
						if(FD_ISSET(fd_ary[i],&f_read))
						{
							while(1)
							{
							ret  = recv(fd_ary[i],buf,BUFFER_SIZE,0);
					    if(ret <=0)
							{
								printf("client %d is out!\n",fd_ary[i]);
								close(fd_ary[i]);
								FD_CLR(fd_ary[i],&f_read);
								fd_ary[i] = 0;
								con_num--;
								break;
							}
							else if(ret >0)
							{
                action_after_recv(buf,ntohs(cliaddr.sin_port));
                send(fd_ary[i],buf,BUFFER_SIZE, 0 );  
							  printf("sent to client::%s\n",buf);
								break;   //这个break超级超级重要，不然就一直会阻塞在这个while循环里
							}
							}  //end while(1)
						}
						}
					}
				}
    for(i =0;i<SETNUM;i++)
    {
      if(fd_ary[i]!= 0)
      {
        close(fd_ary[i]);
      }
    }

  exit(0);

}


int action_after_recv(char * order,int port)
{
	printf("recv from client(%d):: %s\n",port,order);
	return 0;
}
