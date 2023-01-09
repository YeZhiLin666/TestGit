#pragma  once

// #include <stdio.h>
// #include <string.h>
// #include "../../COMMON/Datatypes.h"
// #include "../../HWD/HWDLIB/ADC16bit_AD7656.h"
#define CPLD_BASE_ADDRESS     (0x90000000)
#define CPLD_BASE_ADDR        (CPLD_BASE_ADDRESS)   //�����ַ���������ݲ�ͬ��MCU��ַ���ֲ�ͬ
//������ο��ĵ� ��ַ�ռ�滮
#define NBUS_WIRTE_ARBIN      (CPLD_BASE_ADDR + 0)     //arbin��
#define NBUS_WRITE_ARBINDIR   (CPLD_BASE_ADDR + 2)     //�����
#define NBUS_ARBINWR_RD       (CPLD_BASE_ADDR + 4)     //��ȡ��
#define NBUS_WRITE_CHANNEL    (CPLD_BASE_ADDR + 6)     //ͨ����
#define NBUS_WRITE_DOWRITE    (CPLD_BASE_ADDR + 8)     //
#define NBUS_WRITE_PGA_GAIN   (CPLD_BASE_ADDR + 0x0A)  //�ܵ�PGA
#define NBUS_WRITE_DCS        (CPLD_BASE_ADDR + 0x0C)
#define NBUS_WRITE_VOLTAGERELAY        (CPLD_BASE_ADDR + 0x0E)   //20140329 ��ӵ�ѹ����֧�֣����LELOAD2501���ư�
#define NBUS_WRITE_BUILTINVOLTAGE        (CPLD_BASE_ADDR + 0x10)

#define NBUS_CPLD2_DI            (CPLD_BASE_ADDR + 0xFC)
#define NBUS_CPLD2_DO            (CPLD_BASE_ADDR + 0xFE)

#define NBUS_ARBIN            0x0001
#define NBUS_ARBINDIR         0x0001
#define NBUS_ADWRITE          0x0002
#define NBUS_DOWRITE          0x0001
#define NBUS_BUS1             0x0004


//2012 11 26    ����ʹ�õ�newArbinBus 1126�汾  ʹ�ñ��밴������ʱ�����
//��ͨ��DO�������ʱ��    ADC����ѡ��ʱ��    ��16λͨ����Ϣ����ʱ��

//���ӳ�
#define ARBUS_DELAY(x)          for(uint16 nn=0; nn<x; nn++ );
//Arbin Bus Write
#define cpu_iowr_8(ADDR,DATA)   *((volatile uint8 *)(ADDR)) = (DATA)
#define cpu_iowr_16(ADDR,DATA)  *((volatile uint16 *)(ADDR)) = (DATA)
#define cpu_iowr_32(ADDR,DATA)  *((volatile uint32 *)(ADDR)) = (DATA)
//Arbin bus read
#define cpu_iord_8(ADDR)        *((volatile uint8 *)(ADDR))
#define cpu_iord_16(ADDR)       *((volatile uint16 *)(ADDR))
#define cpu_iord_32(ADDR)       *((volatile uint32 *)(ADDR))

class CPLD_FUNC
{
public:
    static unsigned char  Valid;

    static void           Init       (void);
    static void           WriteDOBus (unsigned char ChannelIdx,unsigned short SetValue);
    static void			  WriteSpecialACIM(unsigned short ChannelIdx,unsigned char SetValue);
    static void           WriteADGBus(unsigned char ChannelIdx,unsigned short SetValue);
// 	static unsigned short ReadDOBus  (void);
// 	static unsigned short ReadDOBus(unsigned char ChannelIdx);
    //����������������ʱ���߼�������

    static void           Write      (unsigned long Address,unsigned short SetValue);
    static unsigned short Read       (unsigned long Address);
    //����������������ֱ�ӵ�ַ������
};



