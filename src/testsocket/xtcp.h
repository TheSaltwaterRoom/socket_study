#pragma once
#include <string>
class XTcp
{
public:
	XTcp();

	int CreateSocket();
	bool Bind(unsigned short port);
	XTcp Accept();
	int Recv(char* buf, int bufsize);
	int Send(const char* buf, int sendsize);
	void Close();
	int sock = 0;
	unsigned short port = 0;
	std::string ip;
};

