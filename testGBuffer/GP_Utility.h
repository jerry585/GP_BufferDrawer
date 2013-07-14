/***********************************************************************************
  Copyright (C), 2010, Beijing 15Th Institute of CETC Co., Ltd.
  File name: GP_Utility.h 																			 
  Author:    created by dubin lin.
  Version: 	 1.0
  Date:  	 2010.12.03 

  Description:    Reworks平台时统服务通用工具类,用于屏蔽不同平台大小端传输问题

  Others:         // 其它内容的说明
  Function List:  // 下文有详细的函数列表和功能描述
     
  History:        
    
    
    1. Date:    2010/12/03  
       Author:  dubin lin
       Modification: 规范化
      
***********************************************************************************/

#include <string.h>
#include "pico/file.hpp"
#include "pico/dir.hpp"
#include "pico/picostorage.hpp"
#include "pico/gen.hpp"
#include "pico/offsetfile.hpp"
#include "pico/os.hpp"

#define _VXWORKS
#define size_t long

//
// 编译时刻大小端检测。
//
// 大多数CPU只支持一种大小端,Itanium(IA64)和MIPS例外.
//
#if defined(__i386) || defined(_M_IX86) || defined (__x86_64) || defined (_M_ARM) || defined(__ARMEL__) || defined(__MIPSEL__)
#   define SJWZ_LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) || defined(__hppa) || defined(__ppc__) || defined(_ARCH_COM)
#   define SJWZ_BIG_ENDIAN
#else
#   error "Unknown architecture"
#endif

//从小端的字节序字符串中读取一个T型值。
template <typename T>
#ifdef _VXWORKS
inline T strReadVal(const void* pBuffer, size_t pos)
#else
inline T strReadVal(const void* pBuffer, size_t pos)
#endif
{
	T retVal;
	char *p = reinterpret_cast<char*>(&retVal);
	for (unsigned int i = 0; i < sizeof(T); ++i)
	{
#ifdef SJWZ_LITTLE_ENDIAN
		p[i] = reinterpret_cast<const char*>(pBuffer)[i];
#else
		p[i] = reinterpret_cast<const char*>(pBuffer)[sizeof(T) - i - 1];
#endif
	}
	return retVal;
}


//
//将类型为T(int,short...)的值保存到pBuffer中。
//
template <typename T>
inline void gpWriteValue(void *pBuffer, T value)
{
	char *p = reinterpret_cast<char*>(&value);
	for (unsigned int i = 0; i < sizeof(T); ++i)
	{
#ifdef SJWZ_LITTLE_ENDIAN
		reinterpret_cast<char*>(pBuffer)[i] = p[i];
#else
		reinterpret_cast<char*>(pBuffer)[i] = p[sizeof(T) - i - 1];
#endif
	}
}

//以小端的字节序向字符串中写入一个T型值。
template <typename T>
inline void strWriteVal(void *pBuffer, T val)
{
	char t[sizeof(T)];
	gpWriteValue<T>(t,val);
	memcpy(pBuffer,t,sizeof(T));
}

// namespace pico{
typedef OffsetFile<File> PFile;

//���ļ���ȡ���ݵ�string�С�
inline bool strfread(std::string& s, PFile& file, size_t size)
{
	s.resize(size);	//�ز�����
	return size == file.read(&s[0],size);
}

inline size_t gp_fread(void* buffer, size_t size, size_t count, PFile& fp)
{
	return fp.read(buffer, size*count) / size;
}

inline size_t gp_fwrite(const void* buffer, size_t size, size_t count, PFile& fp)
{
	fp.write(buffer, size*count);
	return 0;
}

inline int gp_fseek(PFile& fp,long offset,int origin)
{
	//����ֵ�պ���fseek��һ��
	return static_cast<int>(!fp.seek(offset,static_cast<SeekWhence>(origin)));
}

inline int gp_ftell(PFile& fp)
{
	return fp.tell();
}

inline int fclose(PFile& fp)
{
	fp = File();return 0;
}
/*}*/