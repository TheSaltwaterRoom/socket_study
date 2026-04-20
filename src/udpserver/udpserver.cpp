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

#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
#ifdef _WIN32
    static bool first = true;
    if (first)
    {
        first = false;
        WSADATA ws;
        WSAStartup(MAKEWORD(2, 2), &ws);
    }
#endif
    
	unsigned short port = 8080;

	if (argc > 1)
        port = atoi(argv[1]);



	int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock <= 0) {
        cout << "create socket error" << endl;
		return -1;
    }

	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(0);
    if(::bind(sock, (sockaddr*)&saddr, sizeof(saddr)) < 0) {
        cout << "bind error" << endl;
        return -1;
    }
	cout << "udp server start at port " << port << endl;

    listen(sock, 10);

    sockaddr_in client;
    socklen_t len = sizeof(client);
	char buf[1024] = { 0 };
	int re = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&client, &len);

    if(re<=0) {
        cout << "recvfrom fail" << endl;
        return -1;
	}

    cout << "Received message from " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << endl;

	buf[re] = '\0';

    cout << buf << endl;

    sendto(sock, "nihao", 5, 0, (sockaddr*)&client, len);


	getchar();
    return 0;
}
