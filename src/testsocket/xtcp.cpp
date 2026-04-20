#include "xtcp.h"
#ifdef _WIN32
#include <windows.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define  closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
XTcp::XTcp()
{
#ifdef _WIN32
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
#endif
}

int XTcp::CreateSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("create socket failed!\n");
	}

	printf("sock[%d]\n", sock);
	return sock;
}

bool XTcp::Bind(unsigned short port)
{
	if (sock <= 0)
		CreateSocket();
	sockaddr_in saddr;// IPv4 套接字地址结构体
	saddr.sin_family = AF_INET;//IPv4 地址族
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(0);

	if (::bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		printf("bind port %d failed!\n", port);
		return false;
	}
	printf("bind port %d success!\n", port);
	listen(sock, 10);

	this->port = port;
	return true;
}

XTcp XTcp::Accept()
{
	XTcp tcp;
	sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int client = accept(sock, (sockaddr*)&caddr, &len);
	if (client <= 0) return tcp;
	printf("accept client %d\n", client);
	tcp.ip = inet_ntoa(caddr.sin_addr);
	tcp.port = ntohs(caddr.sin_port);
	tcp.sock = client;
	printf("client ip is %s,port is %d\n", tcp.ip.c_str(), tcp.port);
	return tcp;
}

int XTcp::Recv(char* buf, int bufsize)
{
	return recv(sock, buf, bufsize, 0);
}
int XTcp::Send(const char* buf, int size)
{
	int s = 0;
	while (s < size)
	{
		int len = send(sock, buf + s, size - s, 0);
		if (len <= 0)break;
		s += len;
	}
	return s;
}

void XTcp::Close()
{
	if (sock <= 0) return;
	closesocket(sock);
}
