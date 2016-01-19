#include <stdio.h>  
#include <sys/socket.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <stdlib.h>   
#include<fcntl.h>
  
  
#define  CLIENT_PORT ((20001+rand())%65536)  //  define the defualt client port as a random port   
#define  BUFFER_SIZE 255   
#define  REUQEST_MESSAGE "welcome to connect the server.\n"   
  
int flag;  
void  usage(char* name)  
{  
       printf( " usage: %s IpAddr  port number_of_proc               ##number_of_proc >=1 \n " ,name);  
			 return;
}  
  
  
int  cli_single(int cli_argc, char** cli_argv)  
{  
       int  servfd,clifd,length = 0;  
       struct  sockaddr_in servaddr,cliaddr;  
       socklen_t socklen  =   sizeof (servaddr);  
       char  buf[BUFFER_SIZE];  
  
       if ((clifd  =  socket(AF_INET,SOCK_STREAM, 0 ))  <   0 )  
         {  
             printf( " create socket error!\n " );  
             //exit( 1 );  
						 return -1;
       }   
   
       srand(time(NULL)); // initialize random generator   
   
       bzero( & cliaddr, sizeof (cliaddr));  
       cliaddr.sin_family  =  AF_INET;  
       cliaddr.sin_port  =  htons(CLIENT_PORT);  
       cliaddr.sin_addr.s_addr  =  htons(INADDR_ANY);  
  
       bzero( & servaddr, sizeof (servaddr));  
       servaddr.sin_family  =  AF_INET;  
       inet_aton(cli_argv[ 1 ], & servaddr.sin_addr);  
       servaddr.sin_port  =  htons(atoi(cli_argv[2]));  
      // servaddr.sin_addr.s_addr = htons(INADDR_ANY);   
 
			 /*
       if  (bind(clifd, (struct sockaddr* ) &cliaddr, sizeof (cliaddr)) < 0 )  
       {  
              printf( " bind to port %d failure!\n " ,CLIENT_PORT);  
             // exit( 1 );  
							return -1;
       } 
			 */
  
        if (connect(clifd,( struct  sockaddr * ) & servaddr, socklen)  <   0 )  
       {  
              printf( " can't connect to %s!\n ", cli_argv[ 1 ]);  
              //exit( 1 );  
							return -1;
       }   
		   else
			 {
				 printf("connected!\n");
			 }
	//		fcntl(clifd,F_SETFL,O_NONBLOCK);
				int trad_num;
				if(flag ==1)
				{
					trad_num = 10000;  //模拟无线循环
				}
				else
				{
					trad_num =1;
				}

		while(trad_num--)
		{
			memset(buf,'\0',BUFFER_SIZE);
			if(flag == 1)
			{
		   fgets(buf,BUFFER_SIZE,stdin);
			 if(strcmp(buf,"\n")==0) continue;
			}
			else
			 snprintf(buf,sizeof(buf),"%s","I will never say never!"); 
			if(!strncasecmp(buf,"quit",4)|| !strncasecmp(buf,"q",1))
			 {
				 break;
			 }
			 char filename[BUFFER_SIZE];
			 int fd=0;
			 memset(filename ,'\0',BUFFER_SIZE);
			 if(buf[0] == '<' && buf[1] == '<')
			 {
				 fd=open(memcpy(filename,buf+2,strlen(buf)-3),O_RDONLY);
				 if(fd >0)
				 {
				 memset(filename,'\0',BUFFER_SIZE);
				while( read(fd,filename,sizeof(filename)))
				{
          length = send(clifd,filename,strlen(filename),0);
					if (length <0)
					{
						printf("error send data\n");
						//exit(1);
						return -1;
					}
					else
					{
						printf("send sus!!\n");
					}
				}
				 }
				 else
				 {
					 printf("open %s error\n",filename);
					// exit(1);
					 return -1;
				 }
			 }
			 else
			 {
       length = send(clifd,buf,BUFFER_SIZE,0); 
        if  (length < 0)  
        {  
              printf( " error send data\n");  
             // exit( 1 );  
							return -1;
       }  
				else
					{
						printf("send sus!!\n");
					}
 
       }  //end for send

       bzero(buf,BUFFER_SIZE); 
       length  =  recv(clifd, buf, BUFFER_SIZE, 0);  
			 
			 if(length<=0)
			 {
				 printf("error recv!\n");
				 printf("exit\n");
				// exit(1);
				 return -1;
			 }
			 else
			 {
				 printf("recv from server::\n%s\n",buf);
			 }
		}  
			 close(clifd);  
       return 0;  
}


int main(int argc , char ** argv)
{
   if (argc < 4 )  
   {  
      usage(argv[ 0 ]);  
      exit( 1 );  
   }   
	 if(argv[3] == NULL || atoi(argv[3])== 1)
	 {
		 flag =1;
     cli_single(argc,argv);  
	 }
	 else
	 {
		 flag = 0;
   printf("%s client  will start.........................................................\n",argv[3]);
	 int num=atoi(argv[3]);
	 while(num)
	 {
	 int pid = fork();
	 if(pid < 0)
	 {
		 printf("fork error! exit! \n");
		 exit(1);
	 }
	 else if(pid == 0)  //子进程
	 {
      cli_single(argc,argv);  
			return 0;
	 }
	 else
	 {
		 int stat;
		 wait(&stat);
		 printf("the %dth process return code %d!\n",num,stat);
		 num--;
		 continue;
	 }
	 }
	 }
	 return 0;
}
