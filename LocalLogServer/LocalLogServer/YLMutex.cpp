#include "stdafx.h"
#include "YLMutex.h"

YLMutex::YLMutex(const char* strName)
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };

	m_hMutex = CreateMutexA(&sa, FALSE, strName);
	if(m_hMutex == NULL){
		printf("Create Mutex error: %d \n", GetLastError());
	}
}

YLMutex::~YLMutex()
{
	CloseHandle(m_hMutex);
}

bool YLMutex::Lock()
{
	int nRet = WaitForSingleObject(m_hMutex, INFINITE);
	return nRet == WAIT_OBJECT_0 ? true : false;
}

bool YLMutex::Unlock()
{
	return ReleaseMutex(m_hMutex) ? true : false;
}