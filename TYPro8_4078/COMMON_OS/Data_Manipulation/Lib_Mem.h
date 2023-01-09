/*****************************************************
File Name: Lib_Mem.h
Version:		    V1.0
Date:				2013/1/10
Description:
公用函数，C语言数据类型&内存 相互转换方法集
Others:
History：

******************************************************/
#pragma once

typedef union
{
    unsigned char          m_U8;
    signed char            m_S8;
    unsigned short         m_U16;
    signed short           m_S16;
    unsigned long          m_U32;
    signed long            m_S32;
    unsigned long long     m_U64;
    signed long long       m_S64;

    float                  m_float;
    double                 m_double;
} Un_64Bit;

void SetBit(unsigned char *p_mem,unsigned long Idx,unsigned char bHigh);
bool CheckBit(unsigned char *p_mem,unsigned long Idx);

// unsigned short Mem_to_U16(unsigned char  *p_mem);
// unsigned short Mem_to_S16(unsigned char  *p_mem);
// unsigned long  Mem_to_U32(unsigned char  *p_mem);
// signed long    Mem_to_S32(unsigned char  *p_mem);
// float          Mem_to_float(unsigned char  *p_mem);
// double         Mem_to_double(unsigned char  *p_mem);

// void   U16_to_Mem(unsigned short src,unsigned char  *p_mem);
// void   S16_to_Mem(signed short   src,unsigned char  *p_mem);
// void   U32_to_Mem(unsigned long  src,unsigned char  *p_mem);
// void   S32_to_Mem(signed long    src,unsigned char  *p_mem);
// void   float_to_Mem(float src,unsigned char  *p_mem);
// void   FP64_to_Mem(double src,unsigned char  *p_mem);



