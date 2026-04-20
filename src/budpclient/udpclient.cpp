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
    //设置广播类型
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));

    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_BROADCAST;
	int len = sendto(sock, "hello", 6, 0, (sockaddr*)&saddr, sizeof(saddr));
    cout << "sendto size is " <<  len << endl;
    char buf[1024] = {0};
	recvfrom(sock, buf, 1024, 0, NULL, NULL);
	cout << "recvfrom data is " << buf << endl;

    getchar();
    return 0;
}
