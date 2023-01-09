#pragma  once

// #include <stdio.h>
// #include <string.h>
// #include "../../COMMON/Datatypes.h"
// #include "../../HWD/HWDLIB/ADC16bit_AD7656.h"
#define CPLD_BASE_ADDRESS     (0x90000000)
#define CPLD_BASE_ADDR        (CPLD_BASE_ADDRESS)   //这个地址，具体依据不同的MCU地址划分不同
//具体请参考文档 地址空间规划
#define NBUS_WIRTE_ARBIN      (CPLD_BASE_ADDR + 0)     //arbin脚
#define NBUS_WRITE_ARBINDIR   (CPLD_BASE_ADDR + 2)     //方向脚
#define NBUS_ARBINWR_RD       (CPLD_BASE_ADDR + 4)     //读取脚
#define NBUS_WRITE_CHANNEL    (CPLD_BASE_ADDR + 6)     //通道号
#define NBUS_WRITE_DOWRITE    (CPLD_BASE_ADDR + 8)     //
#define NBUS_WRITE_PGA_GAIN   (CPLD_BASE_ADDR + 0x0A)  //总的PGA
#define NBUS_WRITE_DCS        (CPLD_BASE_ADDR + 0x0C)
#define NBUS_WRITE_VOLTAGERELAY        (CPLD_BASE_ADDR + 0x0E)   //20140329 添加电压驱动支持，配合LELOAD2501控制板
#define NBUS_WRITE_BUILTINVOLTAGE        (CPLD_BASE_ADDR + 0x10)

#define NBUS_CPLD2_DI            (CPLD_BASE_ADDR + 0xFC)
#define NBUS_CPLD2_DO            (CPLD_BASE_ADDR + 0xFE)

#define NBUS_ARBIN            0x0001
#define NBUS_ARBINDIR         0x0001
#define NBUS_ADWRITE          0x0002
#define NBUS_DOWRITE          0x0001
#define NBUS_BUS1             0x0004


//2012 11 26    现在使用的newArbinBus 1126版本  使用必须按照锁存时序操作
//即通道DO输出锁存时序    ADC开关选择时序    和16位通道信息读入时序

//短延迟
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
    //以上两个函数，是时序逻辑操作。

    static void           Write      (unsigned long Address,unsigned short SetValue);
    static unsigned short Read       (unsigned long Address);
    //以上两个函数，是直接地址操作。
};



