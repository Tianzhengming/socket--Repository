#include <stdio.h>
#include<stdlib.h>
//#include <winsock2.h>
//#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
//#define ERR_EXIT(m)\
//do\
//{\
//	perror(m);\
//	exit(EXIT_FAILURE);\
//}while(0)
int main( )
{
	//创建一个套接字
	//int listenfd;
	//if((listenfd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		//ERR_EXIT("socket");
	//初始化一个地址,服务器端为本机地址5188端口
	//struct sockaddr_in servaddr;
	//memset(&servaddr,0,sizeof(servaddr));
	//servaddr.sin_family=AF_INET;
	//servaddr.sin_port=htons(5188);
	//servaddr.sin_addr.s_addr=inet_addr("49.123.93.175");//表示本机地址
	//int on=1;
	//if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
	//	ERR_EXIT("setsockeopt");
	////将套接字与本地地址进行绑定
 //       if((bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	//	ERR_EXIT("bind");
	////将套接字设置为监听状态,为被动套接字用来接受连接
	////somaxconn表示队列的最大值
	//if(listen(listenfd,SOMAXCONN)<0)
	//	ERR_EXIT("listen");
	//struct sockaddr_in peeraddr;
	//socklen_t peerlen=sizeof(peeraddr);
	//int conn;
	////接受连接，conn为主动套接字
	//if((conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0)
	//	ERR_EXIT("accept");
	////打印连接的ip与端口
	//printf("ip=%s port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
	//char recvbuf[1024];
	//while(1)
	//{
	//	memset(recvbuf,0,sizeof(recvbuf));
	//	int ret=read(conn,recvbuf,sizeof(recvbuf));
	//	fputs(recvbuf,stdout);	
	//	write(conn,recvbuf,ret);
	//}
	//close(conn);
	//close(listenfd);
	return 0;
}
