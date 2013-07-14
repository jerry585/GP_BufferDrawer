#ifndef _GP_PERFORMANCE_H_
#define _GP_PERFORMANCE_H_

#include "GP_Time.h"
#include "stdio.h"
#include <map>
#include <string>

#	define gpPerformanceLog(x)  GP_Performance tmpXX__var(x);
#	define gpPerformanceStat(x) GPPerformanceStat(x)
#	define gpPerformanceReset() GPPerformanceReset()

struct TimeRound
{
	GP_Time tmTime;
	int  iRound;
};

struct ltstr	//字符串比较仿函数，用于STL算法
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

typedef std::map<const char*, TimeRound, ltstr> TGP_Performance;

TGP_Performance* GetPerformancePtr();

class GP_Performance
{
public:
	GP_Performance(const char* name):m_tStart(GP_Time::now()),m_szName(name){}
	
	~GP_Performance()	{
		TGP_Performance* p = GetPerformancePtr();
		TGP_Performance::iterator it = p->find(m_szName);
		if (it != p->end())
		{
			(*it).second.tmTime += (GP_Time::now()-m_tStart);
			(*it).second.iRound += 1;
		}
		else
		{
			TimeRound tr;
			tr.tmTime = GP_Time::now()-m_tStart;
			tr.iRound = 1;
			p->insert(std::make_pair(m_szName,tr));
		}
	}	
private:
	GP_Time m_tStart;
	const char* m_szName;
};

inline void GPPerformanceStat(std::string& s)
{
	TGP_Performance* p = GetPerformancePtr();
	char tmp[512];
	for (TGP_Performance::iterator it = p->begin(); it != p->end(); ++it)
	{
		sprintf(tmp,"%s:%.2fs\n",(*it).first, (*it).second.tmTime.toSecondsDouble());
		s.append(tmp);
	}
}
inline void GPPerformanceReset()
{
	TGP_Performance* p = GetPerformancePtr();	
	
	p->clear();
}


#endif