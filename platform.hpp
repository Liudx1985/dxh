///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007, Software R&D Section,FiberHome Telecom. Tech.
// Co.,Ltd
// All rights reserved.
//
// platform.h
//
// Author : Liudx
// Email: Liudx@fiberhome.com.cn
// Date: 2010-8-25
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef __GNUC__
#	include <boost/thread.hpp>
//	extern	int32_t __builtin_bswap32 (int32_t x)
//	extern	int64_t __builtin_bswap64 (int64_t x)
#elif defined(_MSC_VER )
#	include <windows.h>
#	include "intrin.h" // For windows platform declares.
//	//For 16 bit numbers:
//	extern	unsigned short _byteswap_ushort(unsigned short value);
//	// For 32 bit numbers:
//	extern	unsigned long _byteswap_ulong(unsigned long value);
//	// For 64 bit numbers:
//	extern	unsigned __int64 _byteswap_uint64(unsigned __int64 value);
#endif

namespace ns_platform
{

typedef struct tagCPU
{
    int type_;     // process type
    int family_;   // family of the processor
    int model_;    // model of processor
    int stepping_; // processor revision number
    int ext_model_;
    int ext_family_;
    bool has_mmx_;
    bool has_sse_;
    bool has_sse2_;
    bool has_sse3_;
    bool has_ssse3_;
    bool has_sse41_;
    bool has_sse42_;
    bool is_64bit_;
    std::string cpu_vendor_;
} CPUINFO;


void GetCPUInfo(CPUINFO &stCPUInfo)
{
#if defined(ARCH_CPU_X86_FAMILY)
    int cpu_info[4] = { -1 };
    char cpu_string[0x20];

    // InfoType参数传0调用__cpuid, CPUInfo[0]中返回最大合法的Id值,
    // CPU标识字符串在另外三个数组元素中, 不是顺序存放. 下面的代码
    // 对信息进行可读性排序.
    //
    // 更多信息参见:
    // http://msdn.microsoft.com/en-us/library/hskdteyh.aspx
    __cpuid(cpu_info, 0);
    int num_ids = cpu_info[0];
    memset(cpu_string, 0, sizeof(cpu_string));
    *(reinterpret_cast<int*>(cpu_string)) = cpu_info[1];
    *(reinterpret_cast<int*>(cpu_string+4)) = cpu_info[3];
    *(reinterpret_cast<int*>(cpu_string+8)) = cpu_info[2];

    // 解释CPU特征信息.
    if(num_ids > 0)
    {
        __cpuid(cpu_info, 1);
        stCPUInfo.stepping_ = cpu_info[0] & 0xf;
        stCPUInfo.model_ = ((cpu_info[0] >> 4) & 0xf) + ((cpu_info[0] >> 12) & 0xf0);
        stCPUInfo.family_ = (cpu_info[0] >> 8) & 0xf;
        stCPUInfo.type_ = (cpu_info[0] >> 12) & 0x3;
        stCPUInfo.ext_model_ = (cpu_info[0] >> 16) & 0xf;
        stCPUInfo.ext_family_ = (cpu_info[0] >> 20) & 0xff;
        stCPUInfo.cpu_vendor_ = cpu_string;
        stCPUInfo.has_mmx_ = (cpu_info[3] & 0x00800000) != 0;
        stCPUInfo.has_sse_ = (cpu_info[3] & 0x02000000) != 0;
        stCPUInfo.has_sse2_ = (cpu_info[3] & 0x04000000) != 0;
        stCPUInfo.has_sse3_ = (cpu_info[2] & 0x00000001) != 0;
        stCPUInfo.has_ssse3_ = (cpu_info[2] & 0x00000200) != 0;
        stCPUInfo.has_sse41_ = (cpu_info[2] & 0x00080000) != 0;
        stCPUInfo.has_sse42_ = (cpu_info[2] & 0x00100000) != 0;
        stCPUInfo.is_64bit_ = (cpu_info[3] & 0x20000000) !=0;

    }
#endif
}


// Swap
template<typename T>
inline void swap(T& a, T& b)
{
    T t = a;
    a = b;
    b = t;
}


//-----------------------------------------------------------
// Method:	endianness
// Summary:
//	! Return the current endianness of the CPU architecture.
// Parameter:
// Return:
//   [Succeed]: return  false for "Little Endian", true for "Big Endian".
// let int a = 0xAB CD [hex] 'CD' is LSB, 'AB' is MSB
// &a = 0x100
//			0x100	0x101..
// big Endian: AB	CD
// lit endian: CD   AB
//   [Fail]:  never fail
//-----------------------------------------------------------
inline bool Endianness()
{
    const int x = 0x1;
    return ((unsigned char*)&x)[0]?false:true;
	// unsigned short test = 0x1122;
	// return (*( (unsigned char*) &test ) == 0x11);
}

//-----------------------------------------------------------
// Method: endian_swap
// Summary:
//	! Invert endianness of a memory buffer.
// Parameter:
//	 buffer	[IN] : the buffer to invert.
//   size   [IN] : the number of buffer elements. for a single T a, you should call "endian_swap(&a,1)";
//	 	[OUT]:
// Return: none.
// Remark :
//  A byte may look like a:[12345678], a WORD may look like [a][b] in little-endian,
//  [b][a] in big-endian, but for each a and b, the bit order is always [12345678].
//  So we call it byte-order ,not bit-order.
//-----------------------------------------------------------

template<typename T>
inline void endian_swap(T* const buffer, const unsigned int size)
{
    if (size)
        switch (sizeof(T))
        {
        case 1 :
            break; // Notice the a byte order if fixed, Must be lager then 2.
        case 2 :
        {
            for (unsigned short *ptr = (unsigned short*)buffer+size; ptr>(unsigned short*)buffer; )
            {
                const unsigned short val = *(--ptr);
                *ptr = (unsigned short)((val>>8)|((val<<8)));
            }
        }
        break;
        case 4 :
        {
            for (unsigned int *ptr = (unsigned int*)buffer+size; ptr>(unsigned int*)buffer; )
            {
                const unsigned int val = *(--ptr);
                *ptr = (val>>24) | ((val>>8)&0xff00) | ((val<<8)&0xff0000) | (val<<24);
            }
        }
        break;
        default :
        {
            for (T* ptr = buffer+size; ptr>buffer; )
            {
                unsigned char *pb = (unsigned char*)(--ptr), *pe = pb + sizeof(T);
                for (int i = 0; i<(int)sizeof(T)/2; ++i)
                    swap(*(pb++),*(--pe));
            }
        }
        break;
        }
}

// Same as x = _byteswap_ushort(x)
inline short SwapEndian(short shVal)
{
#ifdef _MSC_VER
	_asm {
		mov ax, v
			xchg ah, al;
		mov v, ax;
	}
	return shVal;
#endif 
	return (shVal >> 8) | (shVal << 8);
}


// Same as x = _byteswap_ulong(x)
inline int SwapEndian(int iVal)
{
#ifdef _MSC_VER
	_asm {
		mov eax,v;
		bswap eax;
		mov v, eax;
	}
	return iVal;
#endif
    return (iVal >> 24) |
        ((iVal << 8) & 0x00FF0000) |
        ((iVal >> 8) & 0x0000FF00) |
        (iVal << 24);
}

// __int64 for MSVC, "long long" for gcc
// Same as x = _byteswap_uint64(x)
inline void SwapEndian(unsigned __int64& x)
{
    x = (x>>56) |
        ((x<<40) & 0x00FF000000000000) |
        ((x<<24) & 0x0000FF0000000000) |
        ((x<<8)  & 0x000000FF00000000) |
        ((x>>8)  & 0x00000000FF000000) |
        ((x>>24) & 0x0000000000FF0000) |
        ((x>>40) & 0x000000000000FF00) |
        (x<<56);
}


inline unsigned int cpu_number(void)
{
#ifdef __GNUC__
    return boost::thread::hardware_concurrency();
#elif defined(_WIN32)
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#endif
}

inline bool is64bitSystem()
{
#if defined(_WIN32)
    SYSTEM_INFO  si;
    GetSystemInfo(&si);

    return (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || // IA
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || // AMD
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64  // Alpha
           );
#else
    return false;
#endif
}
// -end of ns_platform
};


#if 0 // -TEST
#include "platform.h"
#include <iostream>
#include <cmath>
//#include <limits>
int main()
{
    std::cout << ns_platform::endianness()
              << std::endl;
    unsigned int a = 2;

    ns_platform::SwapEndian(a);

    std::cout << (a) << std::ends
              << std::endl;

    // swap back :
#ifdef __GNUC__
    a = __builtin_bswap32(a);
#else
    a = _byteswap_ulong(a);
#endif

    std::cout << (a) << std::ends
              << std::endl;
    return 0;
}

#endif // -TEST
