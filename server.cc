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
#include <ctime>
////////////////////////////////
#define PORT 80
#define HOST "127.0.0.1"

void erro(const char *str,UINT exitcode);
void erro(const char *str);
void init(std::vector<char> buff,char (&verb)[5],char (&pth)[200]);
void parse(char (&mtype)[50],char (&type)[20],char (&pth)[200]);
std::vector<char> loadfile(char (&pth)[200]);
void sendata(SOCKET n_socket,char (&status_code)[4],char (&mtype)[50],std::vector<char> dataa);
int get(SOCKET n_socket,char (&pth)[200]);
int post(SOCKET n_socket,char (&recvbuf)[0x1024]);
int connection(SOCKET n_socket);

//formaty: .html .gif .ico .css .js
// resp on get i push
// tylko o 24

//g++ server.cc -o serwer.exe -lws2_32 -std=c++11 -pthread


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
		time_t tt;
    	time(&tt);
    	tm TM = *localtime( &tt );


		if (TM.tm_hour != 24)
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



	}

	 

	return 0;


}


int connection(SOCKET n_socket){

	char recvbuf[0x1024];
	int  recvbuflen = 0x1024;
	bool req = false;
	const char GET[4] = "GET";
	const char POST[5] = "POST";
	
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

 	char verb[5] = " ";
 	char pth[200] = " ";

 	init(finalbuf,verb,pth);

 	int getorpost = 50;

 	for (int i = 0,j=1; i < 4; ++i){

 		if (verb[j] == GET[i])
 		{
 			getorpost++;
 		}

 		if (verb[j] == POST[i])
 		{
 			getorpost--;
 		}
 		j++;
 		
 	}


 	if (getorpost == 46){
 		post(n_socket,recvbuf);
 		return 0;
 	}

 	if(getorpost != 53){
 		closesocket(n_socket);
 		return 3;

 	}


 	get(n_socket,pth);



	return 0;
}



void erro(const char *str,UINT exitcode){
	printf("%s\n",str);
	ExitProcess(exitcode);
}
void erro(const char *str){
	printf("%s\n",str);
}

void init(std::vector<char> buff,char (&verb)[5],char (&pth)[200]){

 	bool flag = true;
	int itr = 0;
 	for(auto x: buff){ 
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
 			continue;

 			
 		}
 		

 	}

}

void parse(char (&mtype)[50],char (&type)[20],char (&pth)[200]){
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
	

}

int post(SOCKET n_socket,char (&recvbuf)[0x1024]){

	char buff[20] =  " ";

	for (int i = 0; i < sizeof(recvbuf); ++i)
	{

		if (recvbuf[i] == '\n' && recvbuf[i+1] == 'Y') 
		{
			for (int z = 0,y = i; z < 20; ++z)
			{
				buff[z] = recvbuf[y++];
			}
		}


	}

	srand(time(NULL));
	int r  = rand();
	std::string name = std::to_string(r);

	std::ofstream revng(name);
  	revng << buff;

  	revng.close();

 	closesocket(n_socket);
 	return 0;

}
int get(SOCKET n_socket,char (&pth)[200]){


std::map<char*, char*> mime = {
    { ".ico","image/vnd.microsoft.icon"},
    { ".gif", "image/vnd.sealedmedia.softseal.gif" },
    { ".js", "text/javascript"},
    { ".css","text/css"},
    { ".html", "text/html;charset=utf-8"}
				};

	char mtype[50] = " ";
	char type[20] = "";
	char status_code[4] = "200";
	parse(mtype,type,pth);
	
	std::vector<char> dataa = loadfile(pth);

	//Check mime type 
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

  	sendata(n_socket,status_code,mtype,dataa);
  	return 0;

} 

std::vector<char> loadfile(char (&pth)[200]){

    std::ifstream input(pth, std::ios::binary);

    std::vector<char> data(
         (std::istreambuf_iterator<char>(input)),
         (std::istreambuf_iterator<char>()));

    input.close();

    return data;

}

void sendata(SOCKET n_socket,char (&status_code)[4],char (&mtype)[50],std::vector<char> dataa){

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

send(n_socket,(const char*)s.c_str(), (int)strlen((const char*)s.c_str()), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);

	strcpy(request,"\r\n");
send(n_socket,request, (int)strlen(request), 0);


	send(n_socket, (const char*)dataa.data(),  dataa.size(), 0);


	closesocket(n_socket);

}

