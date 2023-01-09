/*****************************************************
File Name: Lib_Mem.cpp
Version:		    V1.0
Date:				2013/1/10
Description:
公用函数，C语言数据类型&内存 相互转换方法集
Others:
History：

******************************************************/
#include "includes.h"

void SetBit(unsigned char *p_mem,unsigned long Idx,unsigned char bHigh)
{
    unsigned long Idx_char,Idx_bit;

    Idx_char = Idx / 8;
    Idx_bit = Idx % 8;

    if(bHigh)
        p_mem[Idx_char] |= (0x01 << Idx_bit);
    else
        p_mem[Idx_char] &= ~(0x01 << Idx_bit);
}
bool CheckBit(unsigned char *p_mem,unsigned long Idx)
{
    unsigned long Idx_char,Idx_bit;

    Idx_char = Idx / 8;
    Idx_bit = Idx % 8;

    if((p_mem[Idx_char]>>Idx_bit)&0x01)
        return true;
    else
        return false;
}

// unsigned short Mem_to_U16(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,2);

// 	return un_Temp.m_U16;
// }
// unsigned short Mem_to_S16(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,2);

// 	return un_Temp.m_S16;
// }
// unsigned long  Mem_to_U32(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,4);

// 	return un_Temp.m_U32;
// }
// signed long    Mem_to_S32(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,4);

// 	return un_Temp.m_S32;
// }
// float          Mem_to_float(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,4);

// 	return un_Temp.m_float;
// }
// double         Mem_to_double(unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;

// 	memcpy(&un_Temp.m_double,p_mem,8);

// 	return un_Temp.m_double;
// }

// void   U16_to_Mem(unsigned short src,unsigned char  *p_mem)
// {
// 	*p_mem     = src;
// 	*(p_mem+1) = src>>8;
// }
// void   S16_to_Mem(signed short   src,unsigned char  *p_mem)
// {
// 	*p_mem     = src;
// 	*(p_mem+1) = src>>8;
// }
// void   U32_to_Mem(unsigned long  src,unsigned char  *p_mem)
// {
// 	*p_mem     = src;
// 	*(p_mem+1) = src>>8;
// 	*(p_mem+2) = src>>16;
// 	*(p_mem+3) = src>>24;
// }
// void   S32_to_Mem(signed long    src,unsigned char  *p_mem)
// {
// 	*p_mem     = src;
// 	*(p_mem+1) = src>>8;
// 	*(p_mem+2) = src>>16;
// 	*(p_mem+3) = src>>24;
// }
// void   float_to_Mem(float src,unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;
// 	un_Temp.m_float = src;

// 	*p_mem           = un_Temp.m_U32;
// 	*(p_mem+1)       = un_Temp.m_U32>>8;
// 	*(p_mem+2)       = un_Temp.m_U32>>16;
// 	*(p_mem+3)       = un_Temp.m_U32>>24;
// }
// void   FP64_to_Mem(double src,unsigned char  *p_mem)
// {
// 	Un_64Bit un_Temp;
// 	un_Temp.m_double = src;

// 	*p_mem			 = un_Temp.m_U64;
// 	*(p_mem+1)		 = un_Temp.m_U64>>8;
// 	*(p_mem+2)		 = un_Temp.m_U64>>16;
// 	*(p_mem+3)		 = un_Temp.m_U64>>24;
// 	*(p_mem+4)		 = un_Temp.m_U64>>32;
// 	*(p_mem+5)		 = un_Temp.m_U64>>40;
// 	*(p_mem+6)		 = un_Temp.m_U64>>48;
// 	*(p_mem+7)		 = un_Temp.m_U64>>56;
// }


