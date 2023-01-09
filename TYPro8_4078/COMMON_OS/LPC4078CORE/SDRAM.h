/*****************************************************************************
// 程序文件      :SDRAM.cpp     主要用于存放没有放入特定文件的类程序
// 文件描述      :为LPC4078 MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

#ifndef _SDRAM_H_
#define _SDRAM_H_


class SDRAMFunc
{
private:

public:
    static unsigned char MyIOCount;
    static const unsigned char MyIO[57][4];

    static void SDRAM_Init( void );
    static unsigned char SDRAM_Check(unsigned long int Address);
};
#endif

