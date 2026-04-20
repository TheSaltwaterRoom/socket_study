// xsyslog.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "xudp.h"
#include <regex>
#include <string>
using namespace std;
int main(int argc, char* argv[])
{
	unsigned short port = 514;

	XUdp syslog;
	if (!syslog.Bind(port)) {
		return -1;
	}

	char buf[1024] = {0};
	for (;;) {
		int len = syslog.Recv(buf, sizeof(buf));
		if(len <=0) {
			continue;
		}
		buf[len] = '\0';

		cout << buf << endl;

		//Failed password for root from 192.168.60.1 port 9693 ssh2
		string src = buf;

		regex re(R"(Failed password for (\S+) from (\d+\.\d+\.\d+\.\d+) port (\d+))");
		smatch match;

		if (regex_search(src, match, re)) {
			string user = match[1].str();
			string ip = match[2].str();
			string port = match[3].str();

			//cout << "user: " << user << endl;
			//cout << "ip  : " << ip << endl;
			//cout << "port: " << port << endl;
			
			cout << "Warning: User " << user << "@" << ip << "@" << port << " login failed\n";
		}

		
	}

    return 0;
}
