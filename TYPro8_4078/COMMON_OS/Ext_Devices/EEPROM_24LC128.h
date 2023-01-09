
#ifndef _EEPROM_24LC128_H_
#define _EEPROM_24LC128_H_

#define EEPROM_CMD 0xA0
#define EEPROM_PAGESIZE 64


#define AUXBOARDRSNSRARTADDR     0x0030;
#define AUXBOARDCFGSRARTADDR     0x0040;       // 板信息大小：24字节，虽然有8块板，但每块板上是独立存储。
#define AUXBOARDCHANNELCFGADDR	 0x0080;       // 通道信息大小：16*64字节(1K)，虽然有8块板，但每块板上是独立存储。
//  0X04A0 ~  0X3E80    单板上剩余16K-1.2K =14.8K




#define E2PROM_I2C_PORT I2C1  //在这里直接改EEPROM I2C 端口



class E2PROM_24LC128
{
public:

    static unsigned char BoardCfgBit;                    //描述哪个板子被配置了
    static unsigned char WriteBuf[MAXI2CBUFLEN];
    static unsigned char ReadBuf[MAXI2CBUFLEN];

    static unsigned char uc_EEPROM_OnWriteBuf;			  //写缓冲区中有数据
    static unsigned char uc_EEPROM_OnReadBuf;			 //读缓冲区中有数据

public:
    static void MyInit(void);
    static void WriteI2cBuf(UChar8 *Data,UWord16 Len);
    static bool Write24LC128(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short WrLen);
    static bool Read24LC128(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len);
#if  USE_AS_AUXMCU == 1
    static unsigned short WriteRequest(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len);
    static unsigned short ReadRequest(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len);
#endif
};

#endif

