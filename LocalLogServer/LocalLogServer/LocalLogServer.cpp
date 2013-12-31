// LocalLogServer.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//���ڸ�ʽeg:
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
	setlocale(LC_ALL,"Chinese-simplified");//�������Ļ���
	stream.open(strPath.c_str(), ios::app | ios::out);

	//�ļ����
	stream <<"[" << strTime << "]"
		<<"["<< moduleName << "]"
		<< ": " << msg << "\n";
	stream.close();

	//����̨���
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
		int nRecvLen = recv(sockConn,recvBuf,1024,0);//�������ӿͻ��˽�������
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

	closesocket(sockConn);//�ر�socket

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	//�ú����Ĺ����Ǽ���һ��Winsocket��汾
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return 0;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return 0; 
	}


	//����socket��̲���
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);//�������ӵĿɿ��Է���SOCK_STRAM

	SOCKADDR_IN addrSrv;//��ű��ص�ַ��Ϣ��
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//htol�������ֽ���long��ת��Ϊ�����ֽ���
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(g_nPort);//htos�������˿�ת�����ַ���1024���ϵ����ּ���

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//��socket�󶨵���Ӧ��ַ�Ͷ˿���

	listen(sockSrv,10);//�ȴ������е���󳤶�Ϊ5

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);

	while(1)
	{
		SOCKET sockConn=accept(sockSrv,(SOCKADDR*)&addrClient,&len);//����һ���µ��׽�������ͨ�ţ�����ǰ��ļ����׽���
		
		CreateThread( NULL,NULL,RecvThread,&sockConn,0,NULL);
	}

	return 1;
}