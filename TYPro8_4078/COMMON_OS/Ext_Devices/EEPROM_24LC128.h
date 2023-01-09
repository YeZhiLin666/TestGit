
#ifndef _EEPROM_24LC128_H_
#define _EEPROM_24LC128_H_

#define EEPROM_CMD 0xA0
#define EEPROM_PAGESIZE 64


#define AUXBOARDRSNSRARTADDR     0x0030;
#define AUXBOARDCFGSRARTADDR     0x0040;       // ����Ϣ��С��24�ֽڣ���Ȼ��8��壬��ÿ������Ƕ����洢��
#define AUXBOARDCHANNELCFGADDR	 0x0080;       // ͨ����Ϣ��С��16*64�ֽ�(1K)����Ȼ��8��壬��ÿ������Ƕ����洢��
//  0X04A0 ~  0X3E80    ������ʣ��16K-1.2K =14.8K




#define E2PROM_I2C_PORT I2C1  //������ֱ�Ӹ�EEPROM I2C �˿�



class E2PROM_24LC128
{
public:

    static unsigned char BoardCfgBit;                    //�����ĸ����ӱ�������
    static unsigned char WriteBuf[MAXI2CBUFLEN];
    static unsigned char ReadBuf[MAXI2CBUFLEN];

    static unsigned char uc_EEPROM_OnWriteBuf;			  //д��������������
    static unsigned char uc_EEPROM_OnReadBuf;			 //����������������

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

