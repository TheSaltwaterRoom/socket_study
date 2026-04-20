#include "XHttpResponse.h"
#include <string>
#include <regex>
using namespace std;

static FILE* OpenFileReadBinary(const char* path)
{
#ifdef _WIN32
	FILE* fp = nullptr;
	if (fopen_s(&fp, path, "rb") != 0)
		return nullptr;
	return fp;
#else
	return fopen(path, "rb");
#endif
}

bool XHttpResponse::SetRequest(std::string request)
{
	string src = request;

	// 先取第一行
	size_t pos = src.find("\r\n");
	string line;
	if (pos != string::npos)
		line = src.substr(0, pos);
	else
		line = src;

	string pattern = "^([A-Z]+) ([^ ?]+)(?:\\?([^ ]*))? HTTP/1\\.[01]$";
	regex r(pattern);
	smatch mas;

	if (!regex_match(line, mas, r))
	{
		printf("%s failed!\n", pattern.c_str());
		return false;
	}

	string type = mas[1];
	string path = mas[2];
	string query = mas[3].matched ? mas[3].str() : "";
	string filetype = "";

	size_t dotPos = path.find_last_of('.');
	size_t slashPos = path.find_last_of('/');

	if (dotPos != string::npos && (slashPos == string::npos || dotPos > slashPos))
	{
		filetype = path.substr(dotPos + 1); // 不带点
	}

	if (type != "GET")
	{
		printf("Not GET!!!\n");
		return false;
	}
	string filename = path;
	if (path == "/")
	{
		filename = "/index.html";
	}

	string filepath = "www";
	filepath += filename;

	//php-cgi www/index.php id=1 name=xcj >  www/index.php.html
	if (filetype == "php")
	{
		string cmd = "php-cgi ";
		cmd += filepath;
		cmd += " ";
		//query id=1&name=xcj 
		// id=1 name=xcj
		for (int i = 0; i < query.size(); i++)
		{
			if (query[i] == '&') query[i] = ' ';
		}
		cmd += query;

		cmd += " > ";
		filepath += ".html";
		cmd += filepath;

		//printf("%s\n",cmd.c_str());
		system(cmd.c_str());
	}


	fp = OpenFileReadBinary(filepath.c_str());
	if (fp == NULL)
	{
		printf("open file %s failed!\n", filepath.c_str());
		return false;
	}
	//获取文件大小
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, 0);
	//printf("file size is %d\n",filesize);

	if (filetype == "php")
	{
		char c = 0;
		//\r\n\r\n
		int headsize = 0;
		while (fread(&c, 1, 1, fp) > 0)
		{
			headsize++;
			if (c == '\r')
			{
				fseek(fp, 3, SEEK_CUR);
				headsize += 3;
				break;
			}
		}
		filesize = filesize - headsize;
	}

	return true;
}
std::string XHttpResponse::GetHead()
{

	string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n";
	rmsg += "Server: XHttp\r\n";
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	rmsg += std::to_string(filesize);
	//rmsg += 
	//rmsg += "10\r\n";
	rmsg += "\r\n\r\n";
	return rmsg;
}
int XHttpResponse::Read(char* buf, int bufsize)
{
	return fread(buf, 1, bufsize, fp);
}
XHttpResponse::XHttpResponse()
{
}


XHttpResponse::~XHttpResponse()
{
}
