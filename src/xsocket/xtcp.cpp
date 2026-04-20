#include "xtcp.h"
#ifdef _WIN32
#include <windows.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#define  closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

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
#ifdef _WIN32
		printf("bind port %u failed! error=%d\n", port, WSAGetLastError());
#else
		printf("bind port %u failed: %s\n", port, strerror(errno));
#endif
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
	sock = 0;
}

#ifdef _WIN32
void PrintSocketError(const char* ip, int port)
{
	int err = WSAGetLastError();
	char* msg = NULL;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msg,
		0,
		NULL
	);

	printf("connect to %s:%d failed! err=%d, msg=%s\n",
		ip, port, err, msg ? msg : "unknown");

	if (msg) {
		LocalFree(msg);
	}
}
#endif

bool XTcp::Connect(const char* ip, unsigned short port, int timeoutms)
{
	if (sock <= 0)
		CreateSocket();

	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	SetBlock(false);
	fd_set set;

	if (connect(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		FD_ZERO(&set);
		FD_SET(sock, &set);
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;

		if (select(sock + 1, 0, &set, 0, &tm) <= 0)
		{
			printf("select\n");
#ifdef _WIN32
			PrintSocketError(ip, port);
#else
			printf("connect to %s:%d failed!:%s\n", ip, port, strerror(errno));
#endif
			return false;
		}
	}

	SetBlock(true);
	printf("connect to %s:%d success!\n", ip, port);
	return true;
}

bool XTcp::SetBlock(bool isblock)
{
	if (sock <= 0) return false;

#ifdef _WIN32
	unsigned long ul = 0;// 0表示阻塞，1表示非阻塞
	if (!isblock) ul = 1;
	ioctlsocket(sock, FIONBIO, &ul);
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0)
		return false;
	if (isblock)
	{
		flags = flags & ~O_NONBLOCK;
	}
	else
	{
		flags = flags | O_NONBLOCK;
	}
	if (fcntl(sock, F_SETFL, flags) != 0)
		return false;
#endif

	return true;
}
