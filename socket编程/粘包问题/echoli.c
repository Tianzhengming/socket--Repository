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

struct packet
{
 	int len;//包头
	char buf[1024];//包体
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

	
	struct packet sendbuf;
	struct packet recvbuf;
	memset(&sendbuf,0,sizeof(sendbuf));
	memset(&recvbuf,0,sizeof(recvbuf));
	int n;
	//数据传输，客户端将数据写给套接字，客户端与服务器端通过套接字进行通信
	while(fgets(sendbuf.buf,sizeof(sendbuf.buf),stdin) != NULL)
	{	
		n=strlen(sendbuf.buf);//包体长度
		sendbuf.len=htonl(n);//将包体长度转换成四个字节序放入包头
		writen(sock,&sendbuf,4+n);//将包体包头一共4+n个字节序发送
		//读取操作
		int ret=readn(sock,&recvbuf.len,4);//先将四个字节序读入包头
		if(ret==-1)//如果字节序为-1读入失败
		{
			ERR_EXIT("read");		
		}
		else if(ret<4)//如果字节序小于4,客户端为传完就关闭
		{
			printf("client close\n");
			break;
		}
		n=ntohl(recvbuf.len);//将包头中的数据转换成整数得到包体数据的大小
		ret=readn(sock,recvbuf.buf,n);//读入包体数据
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
		memset(&sendbuf,0,sizeof(sendbuf));
		memset(&recvbuf,0,sizeof(recvbuf));
	}
	close(sock);
	return 0;
}
