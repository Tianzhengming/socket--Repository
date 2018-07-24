#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#define ERR_EXIT(m)\
do\
{\
	perror(m);\
	exit(EXIT_FAILURE);\
}while(0)
int main(void)
{
	//创建套接字
	int sock;
	if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		ERR_EXIT("socket");
	//初始化一个将要连接对方的地址ipv4
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(5188);
	servaddr.sin_addr.s_addr=inet_addr("49.123.93.175");//表示本机地址
	//发起连接从逻辑上看sock与服务器端conn套接字进行连接
	//sock连接后自动选择一个端口
	if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		ERR_EXIT("connect");
	char sendbuf[1024]={0};
	char recvbuf[1024]={0};
	//数据传输，客户端将数据写给套接字，客户端与服务器端通过套接字进行通信
	while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
	{	
		write(sock,sendbuf,strlen(sendbuf));
		read(sock,recvbuf,sizeof(recvbuf));
		fputs(recvbuf,stdout);
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
	}
	close(sock);
	return 0;
}
