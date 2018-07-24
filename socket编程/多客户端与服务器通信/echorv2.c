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
//服务器通信函数
void do_service(int conn)
{
	char recvbuf[1024];
	while(1)
	{
		memset(recvbuf,0,sizeof(recvbuf));
		int ret=read(conn,recvbuf,sizeof(recvbuf));
		//当ret为0时表示客户端关闭跳出循环
		if(ret==0)
		{
			printf("client close\n");
			break;
		}
		else if(ret==-1)
			ERR_EXIT("read");
		fputs(recvbuf,stdout);	
		write(conn,recvbuf,ret);
	}
}
int main(void)
{
	//创建一个套接字
	int listenfd;
	if((listenfd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		ERR_EXIT("socket");
	//初始化一个地址,服务器端为本机地址5188端口
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(5188);
	servaddr.sin_addr.s_addr=inet_addr("49.123.93.175");//表示本机地址
	int on=1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		ERR_EXIT("setsockeopt");
	//将套接字与本地地址进行绑定
        if((bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
		ERR_EXIT("bind");
	//将套接字设置为监听状态,为被动套接字用来接受连接
	//somaxconn表示队列的最大值
	if(listen(listenfd,SOMAXCONN)<0)
		ERR_EXIT("listen");
	struct sockaddr_in peeraddr;
	socklen_t peerlen=sizeof(peeraddr);
	int conn;
	pid_t pid;
	while(1)
	{
		//接受连接，conn为主动套接字
		if((conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0)
			ERR_EXIT("accept");
		//打印连接的ip与端口
		printf("ip=%s port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
		//创建一个子进程
		pid=fork();
		if(pid==-1)
			ERR_EXIT("fork");
		//在子进程中处理通信，在父进程中进行监听
		if(pid==0)
		{
			close(listenfd);
			do_service(conn);
			//客户端关闭后跳出循环后关闭子进程
			exit(EXIT_SUCCESS);
		}
		else close(conn);
	}
	close(conn);
	close(listenfd);
	return 0;
}
