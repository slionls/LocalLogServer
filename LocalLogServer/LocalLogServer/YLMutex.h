#pragma once
#include "stdafx.h"

class YLMutex 
{
public:
	// YLMutex(const char* strName = NULL);
	// 改成进程间的锁,修改于-6-07-13
	YLMutex(const char* strName = "LangHua_GAMEBOX_2010-04-25");
	virtual ~YLMutex();
	bool Lock();
	bool Unlock();
private:
	HANDLE m_hMutex;
};
