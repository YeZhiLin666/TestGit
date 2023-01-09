#ifndef __LM75B_H
#define __LM75B_H


#define USE_LM75B     0
#define LM75_CMD 0x90
#define LM75_I2C_PORT I2C1
typedef struct
{
    unsigned char Reserved:3;
    unsigned char OS_F_QUE:2;
    unsigned char OS_POL:1;
    unsigned char OS_COMP_INT:1;
    unsigned char SHUTDOWN:1;

} LM75ConfReg;
typedef union
{
    LM75ConfReg ConfigBit;
    unsigned char uc_Config;
} un_ConfigReg;

typedef enum
{
    OS_COMPARATOR = 0,
    OS_INTERRUPT = 1,
} OSMode;
typedef enum
{
    OS_ACTIVE_LOW = 0,
    OS_ACTIVE_HIGH = 1,
} OSPolarity;
typedef enum
{
    POWER_NORMAL  = 0,
    POWER_SHUTDOWN = 1,
} PowerMode;

enum LM75BREG
{
    TEMP_REG_POINT = 0x00,
    CONFIG_REG_POINT = 0x01,
    THYST_REG_POINT = 0x02,
    TOS_REG_POINT  = 0x03,
};

class LM75B
{
public:
    static float ReadTempf(unsigned char adr);
    //    static bool ReadTempReq(unsigned char adr,float& fTemp);  //∂¡Œ¬∂»«Î«Û
    //    static bool SetTosTemp(unsigned char adr,float TempTos);
    //    static bool SetThystTemp(unsigned char adr,float TempThyst);
    //    static float ReadThystTemp(unsigned char adr);
    //    static float ReadTosTemp(unsigned char adr);

    static unsigned char ReadConfig(unsigned char adr);
    static bool WriteConfig(unsigned char adr,un_ConfigReg ConfigReg);
    //    static void setPowerMode(unsigned char adr,PowerMode mode);

    //    static OSMode getOsMode(unsigned char adr);
    //    static void setOsMode(unsigned char adr,OSMode mode);
    //    static OSPolarity getOsPolarity(unsigned char adr);
    //    static void setOsPolarity(unsigned char adr,OSPolarity polarity);

    static unsigned char bAssigned[8];
    static unsigned short HandleID[8];
};

#endif
