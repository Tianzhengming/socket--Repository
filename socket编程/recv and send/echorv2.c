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
//定义一个包的结构
struct packet
{
 	int len;
	char buf[1024];
};
ssize_t readn(int fd,void *buf,size_t count)
{
	size_t nleft=count;//代表剩余读取的字节数
	ssize_t nread;
	char *bufp=(char*)buf;
	while(nleft>0)
	{
		if((nread=read(fd,bufp,nleft))<0)
		{
			if(errno==EINTR)
				continue;
			return -1;		
		}
		//如果读取打字节数等于0结束退出循环
		else if(nread==0)
		{
			break;	
			return (count-nleft);	
		}
		else
		{
			bufp+=nread;
			nleft-=nread;		
		}
	}
	return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft=count;//代表剩余发送的字节数
	ssize_t nwritten;
	char *bufp=(char*)buf;
	while(nleft>0)
	{
		//如果出现信号中断继续循环
		if((nwritten=write(fd,bufp,nleft))<0)
		{
			if(errno==EINTR)
				continue;
			return -1;		
		}
		//如果缓冲区数据等于0继续循环
		else if(nwritten==0)
		{
			continue;
		}
		else
		{
			bufp+=nwritten;
			nleft-=nwritten;
		}
	}
	return count;
}
//服务器通信函数
void do_service(int conn)
{
	struct packet recvbuf;
	int n;
	while(1)
	{
		memset(&recvbuf,0,sizeof(recvbuf));
		//读入包头数据
		int ret=readn(conn,&recvbuf.len,4);
		if(ret==-1)
		{
			ERR_EXIT("read");		
		}
		else if(ret<4)
		{
			printf("client close\n");
			break;
		}
		//将包头数据转换成整数得知包体长度
		n=ntohl(recvbuf.len);
		//读入包体数据
		ret=readn(conn,recvbuf.buf,n);
		if(ret==-1)
		{
			ERR_EXIT("read");		
		}
		else if(ret<n)
		{
			printf("client close\n");
			break;
		}
		fputs(recvbuf.buf,stdout);
		//将收到的4+n个字节数据回射回去	
		writen(conn,&recvbuf,4+n);
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
