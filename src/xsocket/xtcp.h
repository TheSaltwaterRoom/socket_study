#ifndef XTCP_H
#define XTCP_H

	#ifdef _WIN32
		#ifdef XSOCKET_EXPORTS
		#define XSOCKET_API __declspec(dllexport)
		#else
		#define XSOCKET_API __declspec(dllimport)
		#endif
	#else
		#define XSOCKET_API
	#endif



#include <string>
class XSOCKET_API XTcp
{
public:
	XTcp();

	int CreateSocket();
	bool Bind(unsigned short port);
	XTcp Accept();
	int Recv(char* buf, int bufsize);
	int Send(const char* buf, int sendsize);
	bool Connect(const char* ip, unsigned short port, int timeoutms = 1000);
	bool SetBlock(bool isblock);
	void Close();
	int sock = 0;
	unsigned short port = 0;
	std::string ip;
};
#endif

