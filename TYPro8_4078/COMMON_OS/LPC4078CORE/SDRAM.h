/*****************************************************************************
// �����ļ�      :SDRAM.cpp     ��Ҫ���ڴ��û�з����ض��ļ��������
// �ļ�����      :ΪLPC4078 MCU board����
// ��д����      :2012.07.11
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

