// LocalLogServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "YLMutex.h"

YLMutex	g_Mutex;

const string g_strLogFile = "C:\\LogServer.txt";
const int g_nPort = 6001;

string FormatTime(const SYSTEMTIME& time)
{
	string strDay, strTime ;
	stringstream stream;
	char fillc = stream.fill('0');

	stream<< setw(2) << time.wMonth << "-" << setw(2) << time.wDay;
	stream>> strDay;
	stream.clear();

	stream<< setw(2) << time.wHour << ":" 
		<< setw(2) << time.wMinute << ":" 
		<< setw(2) << time.wSecond << "."
		<< setw(3) << time.wMilliseconds;
	stream>> strTime;
	strTime = strDay + " " + strTime;
	//日期格式eg:
	//04-01 10:40:10.265
	return strTime;
}

void Log(const string& strPath, const string& moduleName, const string& msg)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	string strTime = FormatTime(time);

	g_Mutex.Lock();

	ofstream stream;
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	stream.open(strPath.c_str(), ios::app | ios::out);

	//文件输出
	stream <<"[" << strTime << "]"
		<<"["<< moduleName << "]"
		<< ": " << msg << "\n";
	stream.close();

	//控制台输出
	cout <<"[" << strTime << "]"
		<<"["<< moduleName << "]"
		<< ": " << msg << "\n";

	setlocale(LC_ALL,"C");

	g_Mutex.Unlock();
}

string PraseRecvData(const int nUnique, const char *csRecvData, const int nDataLen)
{
	const char *csLog = csRecvData;
	char csBuf[16] = {0};
	sprintf_s(csBuf,"Unique:%d",nUnique);
	string strExtra;
	for (int i=0; i<nDataLen; ++i)
	{
		if (csRecvData[i] == '\0')
		{
			Log(g_strLogFile,csBuf, csLog);
			csLog = NULL;
			if (i+1 < nDataLen)
			{
				csLog = &csRecvData[i+1];
			}
		}
	}
	if (csLog != NULL)
	{
		int len = nDataLen-(csLog-csRecvData);
		strExtra.append(csLog,len);
	}

	return strExtra;
}

DWORD WINAPI RecvThread(void *ppar)
{
	SOCKET sockConn = *(SOCKET*)ppar;

	printf("start %d\n",sockConn);

	string strExtra;
	while (true)
	{
		char recvBuf[1024] = {0};
		int nRecvLen = recv(sockConn,recvBuf,1024,0);//服务器从客户端接受数据
		if (nRecvLen == 0)
		{
			break;
		}
		if (nRecvLen == SOCKET_ERROR)
		{
			int id = WSAGetLastError();
			cout << "error!!!!!  WSAGetLastError:" << id
				<< " Socket:" << (int)sockConn;
			break;
		}
		strExtra.append(recvBuf,nRecvLen);
		strExtra = PraseRecvData((int)sockConn,strExtra.c_str(),strExtra.length());
	}

	printf("end %d\n",sockConn);

	closesocket(sockConn);//关闭socket

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	//该函数的功能是加载一个Winsocket库版本
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return 0;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return 0; 
	}


	//真正socket编程部分
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);//面向连接的可靠性服务SOCK_STRAM

	SOCKADDR_IN addrSrv;//存放本地地址信息的
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//htol将主机字节序long型转换为网络字节序
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(g_nPort);//htos用来将端口转换成字符，1024以上的数字即可

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//将socket绑定到相应地址和端口上

	listen(sockSrv,10);//等待队列中的最大长度为5

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	while(1)
	{
		SOCKET sockConn=accept(sockSrv,(SOCKADDR*)&addrClient,&len);//建立一个新的套接字用于通信，不是前面的监听套接字
		
		CreateThread( NULL,NULL,RecvThread,&sockConn,0,NULL);
	}

	return 1;
}