#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
using namespace std;
struct rd{
int rtr;
string r;
};
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
string lsi()
{	
	string temp, ls;
	system("ls /home  > /home/ls.txt");
	ifstream fin("/home/ls.txt");
	while(!fin.eof())
	{
		getline(fin, temp);
		ls += temp + "\n";
	}
	fin.close();
	ofstream fout("/home/ls.txt",ofstream::out | ofstream::trunc);
	fout.close();
	ls.erase(ls.length()-5,ls.length());
	return ls;
}
bool safewrite(int newsockfd, const string buffer)
{
	int n = write(newsockfd, buffer.c_str(), buffer.length());
	if (n < 0) error("ERROR writing from socket");
	else if (n == 0) return 0;
	return 1;
}
rd saferead(int newsockfd, size_t size)
{
	char buffer[size];
	rd re;
	memset((char*)&re,0,sizeof(re));
	int n = read(newsockfd,const_cast<char*>(buffer),256);
	if (n < 0) error("ERROR reading from socket");
	else if (n == 0) {re.rtr = 0;return re;}
	else re.rtr = 1;
	re.r = buffer;
	for(int i =0;i<re.r.length();i++) 
	{
		if(re.r[i] == '\r')
		{
			re.r[i] = '\0';
			i = re.r.length()+1;
		}
	}
	return re;
}
bool menu(int newsockfd)
{
	string menu = "1 - List of files\n2 - Download file \n3 - upload file\n4 - delete file\n";
	if(safewrite(newsockfd, menu.c_str()) == 1)
		return 1;
	else return 0;
}
bool escape(const string re)
{
	int o;
	for(int i =0;i<re.length();i++)
	{
		if(re[i] == '\0')
		{
			o = i;
			break;
		}
	}  
	if(re[0] == 'q') 
	{
		if(o == 1)
			return 1;
	}
	return 0;
}
