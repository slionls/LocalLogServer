#pragma once
#include "stdafx.h"

class YLMutex 
{
public:
	// YLMutex(const char* strName = NULL);
	// �ĳɽ��̼����,�޸���-6-07-13
	YLMutex(const char* strName = "LangHua_GAMEBOX_2010-04-25");
	virtual ~YLMutex();
	bool Lock();
	bool Unlock();
private:
	HANDLE m_hMutex;
};
