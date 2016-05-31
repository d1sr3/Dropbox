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
	string tmp, s;
	system("ls /home  > /home/ls.txt");
	ifstream fin("/home/ls.txt");
	while(!fin.eof())
	{
		getline(fin, tmp);
		s += tmp + "\n";
	}
	fin.close();
	ofstream fout("/home/ls.txt",ofstream::out | ofstream::trunc);
	fout.close();
	s.erase(s.length()-5,s.length());
	return s;
}
bool safewrite(int, const string, size_t);
rd saferead(int, char[], size_t);
bool menu(int);
bool escape(const string);
int main()
{
	rd re;
	bool brk = 0, inbrk = 0;
	int sockfd, newsockfd, portno = 80, n, o;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	string temp, tempB, ls = "", tempx; 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
		listen(sockfd,5);
	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		while(1)
		{
			if(menu(newsockfd)== 0) break;
			re = saferead(newsockfd,const_cast<char*>(temp.c_str()),1);
			if(re.rtr == 0) break;
			if(escape(re.r) == 1)
			{
				inbrk = 1;
				break;
			}
			if(re.r[0] == '1')
			{	
				string l = lsi();
				if(safewrite(newsockfd,l.c_str(),l.length()) == 0) break;
			}
			else if(re.r[0] == '2')
			{
				string f = "";
				string g = "";
				if(safewrite(newsockfd,"Give me the file location : ",29) == 0) break;
				re = saferead(newsockfd,const_cast<char*>(tempB.c_str()),256);
				if(re.rtr == 0) break;
				if(escape(re.r) == 0)
				{
					ifstream fin(re.r.c_str());
					if (fin.is_open())
					{
						fin.clear();
						while(!fin.eof())
						{
							getline(fin, f);
							f+=g + "\n";
						}
						fin.close();
						if(safewrite(newsockfd, f.c_str(), f.length()) == 0) break;
					}
				}
			}
			else if(re.r[0] == '3')
			{
				bool stp = 0;
				string temp = "";
				if(safewrite(newsockfd, "Select filename : ", 18) == 0) break;
				re = saferead(newsockfd,const_cast<char*>(tempB.c_str()),256);
				if(re.rtr == 0) break;
				if(escape(re.r) == 0)
				{
					string name = re.r;
					ls = lsi();
					int i = 0;
					while(ls[i] != '\0')
					{
						if(ls[i] == '\n')
						{
							if(temp == re.r)
							{
								if(safewrite(newsockfd, "Do you really want to rewrite this file?(y/n) : ", 48) == 0)
								{
									brk = 1;
									break;
								}
								re = saferead(newsockfd,const_cast<char*>(tempx.c_str()),1);
								if(re.rtr = 0)
								{
									brk = 1;
									break;
								}
								if(re.r[0] == 'n')
								{
									stp = 1;
									break;
								}
								else
									break;
							}
							else
								temp = "";
						}
						else
							temp += ls[i];
						i++;
					}
					if(brk == 1)
					{
						brk = 0;
						break;
					}
					if(stp == 0)
					{
						ofstream fout(name.c_str(),ofstream::out | ofstream::trunc);
						fout.close();
						string ls = "";
						if(safewrite(newsockfd, "Select file to upload : ", 24) == 0)
						{
							brk = 1;
							break;
						}
						string outs = "";
						while(1)
						{
							memset(&re,'\0',sizeof(rd));
							re = saferead(newsockfd,const_cast<char*>(tempx.c_str()),256);
							if(re.rtr == 0)
							{
								brk = 1;
								break;
							}
							if(escape(re.r) == 0)
							{
								for(int i =0;i<re.r.length();i++)
								{
									if(re.r[i] == '\0')
									{
										o = i;
										break;
									}
									outs += re.r[i];
								}  
								if(re.r[0] == 's') 
								{
									if(o == 1)
										break;
								}
							}
							else break;
						}	
						if(brk == 1)
						{
							brk = 0;
							break;
						}
						if(escape(re.r) == 0)
						{
							outs.erase(outs.length()-1);
							ofstream fot(name.c_str(),ofstream::out | ofstream::app);
							if (fot.is_open())
							{
								fot << outs;
								fot.close();
							}
						}
					}
					stp = 0;
				}
			}
			else if(temp[0] == '4')
			{
				memset(&re,'\0',sizeof(rd));
				if(safewrite(newsockfd, "Select filename : ", 18) == 0) break;
				re = saferead(newsockfd,const_cast<char*>(tempB.c_str()),256);
				if(re.rtr == 0) break;
				if(escape(re.r) == 0)
				{
					//for(int i =0;i<re.r.length();i++) cout << re.r[i] << endl;
					re.r = "rm " + re.r;
					system(re.r.c_str());
				}
			}
		}
		if(inbrk == 1)
			break;
	}
		close(newsockfd);
		close(sockfd);

	return 0;
}
bool safewrite(int socke, const string buffer, size_t size)
{
	int n = write(socke, buffer.c_str(), size);
	if (n < 0) error("ERROR writing from socket");
	else if (n == 0) return 0;
	return 1;
}
rd saferead(int socke, char buffer[], size_t size)
{
	rd rem;
	memset((char*)&rem,0,sizeof(rem));
	int nm = read(socke,buffer,size);
	if (nm < 0) error("ERROR reading from socket");
	else if (nm == 0) rem.rtr = 0;
	nm = read(socke,buffer,size);
	if (nm < 0) error("ERROR reading from socket");
	else if (nm == 0) rem.rtr = 0;
	else rem.rtr = 1;
	rem.r = buffer;
	for(int i =0;i<rem.r.length();i++) 
	{
		if(rem.r[i] == '\r')
		{
			rem.r[i] = '\0';
			i = rem.r.length()+1;
		}
	}
	return rem;
}
bool menu(int socke)
{
	string menu = "1 - List of files\n2 - Download file \n3 - upload file\n4 - delete file\n";
	if(safewrite(socke, menu.c_str(), menu.length()) == 1)
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
