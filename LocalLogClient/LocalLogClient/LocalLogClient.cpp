// LocalLogClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Winsock2.h>
#include <stdio.h>


int _tmain(int argc, _TCHAR* argv[])
{
	//�̶���ʽ
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

	//����ͨѶsocket
	SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//�趨��Ҫ���ӵķ�������ip��ַ
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6001);//�趨��Ҫ���ӵķ������Ķ˿ڵ�ַ
	int nRet = connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//���������������


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