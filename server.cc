#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iterator>
////////////////////////////////
#define PORT 80
#define HOST "127.0.0.1"


//formaty: .html .gif .ico .css .js
// resp on get i push
// tylko o 24

//g++ server.cc -o serwer.exe -lws2_32 -std=c++11 -pthread


void erro(const char *str,UINT exitcode){
	printf("%s\n",str);
	ExitProcess(exitcode);
}
void erro(const char *str){
	printf("%s\n",str);
}


int connection(SOCKET n_socket){

	char recvbuf[0x1024];
	int  recvbuflen = 0x1024;
	bool req = false;

	recv(n_socket,recvbuf,recvbuflen,0);

	int i = 0;
	std::vector<char> finalbuf;
	for (i; i < recvbuflen; ++i)
	{
		if(recvbuf[i] == '\r' && recvbuf[i + 3] == '\n'){req = true;break;};
		finalbuf.push_back(recvbuf[i]);
	}
	if(!req){
	   erro("Bad Request");
	   closesocket(n_socket);
	   return 3;
	}

 	char verb[200] = " ";
 	char pth[200] = " ";
 	bool flag = true;
	int itr = 0;
 	for(auto x: finalbuf){ 
 		itr++;
 		if (flag)
 		{
 			verb[itr] += x;
 		}
 		else{
 			
 			pth[itr] += x;
 			
 		}
 		if(x == ' ')
 		{
 			if (flag == false)
 			{
 				break;
 			}
 			flag = false;
 			itr = 0;
 			
 		}

 	}

	char mtype[50] = " ";
	if(pth[1] == '/' && pth[2] == ' '){

		strcpy(pth,"index.html");

	}
	else{
		bool frst = true;
		int j = 0;
		for (int i = 0; i < 200; ++i)
		{
			j++;
			if (frst)
			{
				i--;
				frst = false;
				continue;
			}
			if (pth[i] == 0)
			{
				break;
			}
			pth[i] = pth[j];

		}

	}

	char type[20] = "";
	int state = 0;
	int itrto = 0; 
	for (int i = 0; i < 200; ++i)
	{

		if (pth[i] == 0)
		{
			break;
		}
		if(pth[i] == '.'){
			state = 2;

		}
		if(state == 2){
			type[itrto++] += pth[i];
		}

	}  
	

    std::ifstream input(pth, std::ios::binary);

    std::vector<char> dataa(
         (std::istreambuf_iterator<char>(input)),
         (std::istreambuf_iterator<char>()));

    input.close();


	
	std::map<char*, char*> mime = {
    { ".ico","image/vnd.microsoft.icon"},
    { ".gif", "image/vnd.sealedmedia.softseal.gif" },
    { ".js", "text/javascript"},
    { ".css","text/css"},
    { ".html", "text/html;charset=utf-8"}
				};

for (auto& x: mime) {
	if (type[1] == x.first[1])
	{
		strcpy(mtype,x.second);
		break;
	}
	else{
		strcpy(mtype,"application/binary");
	}
    
  }


char request[300] = " ";


	strcpy(request,"HTTP/1.1 ");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"200 OK");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"Content-Type: ");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,mtype);
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"Content-Length: ");
send(n_socket,request, (int)strlen(request), 0);


std::string s = std::to_string(dataa.size());
char const *pchar = s.c_str(); 

send(n_socket,pchar, (int)strlen(pchar), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);


	send(n_socket, (const char*)dataa.data(),  dataa.size(), 0);


	closesocket(n_socket);
	return 0;
}

int main(void)
{

	WORD wVersionRequested;

	WSADATA wsaData;

	int wsaerr;

	wVersionRequested = MAKEWORD(2, 2);

	wsaerr = WSAStartup(wVersionRequested, &wsaData);

	if (wsaerr != 0){erro("Server: The Winsock dll not found!\n",3);};

	SOCKET m_socket;

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	 
	if (m_socket == INVALID_SOCKET){ WSACleanup();erro("Socket Error",3);};

	SOCKADDR_IN servsock;
	servsock.sin_family = AF_INET;
	servsock.sin_addr.s_addr = inet_addr(HOST);
	servsock.sin_port = htons(PORT);

	
	if (bind(m_socket, (SOCKADDR*)&servsock, sizeof(servsock)) == SOCKET_ERROR)
	{	closesocket(m_socket);
		WSACleanup();
		erro("Bind Error",3);
	}
	 
	if (listen(m_socket, 1) == SOCKET_ERROR)
	{	closesocket(m_socket);
		WSACleanup();
		erro("Listen Error",3);
	}
	
	puts("Server: Waiting for a client to connections...\n");

	

	while(1)
	{
		SOCKET AcceptSocket = accept(m_socket, NULL,NULL);
	    if (AcceptSocket == INVALID_SOCKET) {
	        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
	        closesocket(m_socket);
	        WSACleanup();
	        return 1;
	    } else wprintf(L"Client connected.\n");
	    //std::thread newConnection(connection,AcceptSocket);
	    connection(AcceptSocket);



	}

	 

	return 0;


}

