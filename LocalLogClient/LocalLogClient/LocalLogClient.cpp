// LocalLogClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Winsock2.h>
#include <stdio.h>


int _tmain(int argc, _TCHAR* argv[])
{
	//固定格式
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return 0;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup( );
			return 0; 
	}

	//建立通讯socket
	SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//设定需要连接的服务器的ip地址
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6001);//设定需要连接的服务器的端口地址
	int nRet = connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//与服务器进行连接


	for (int i=0;i<100;i++)
	{
		char Buf[16] = {0};
		sprintf_s(Buf,"%d",i);
		nRet = send(sockClient,Buf,strlen(Buf)+1,0);
	}
	closesocket(sockClient);
	WSACleanup();

	return 0;
}