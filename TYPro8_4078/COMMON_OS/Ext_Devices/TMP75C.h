#ifndef __TMP75C_H
#define __TMP75C_H


#define USE_TMP75C     0
#define TMP75C_CMD 0x90
#define TMP75C_I2C_PORT I2C1
typedef struct
{
    unsigned char Reserved:3;
    unsigned char OS_F_QUE:2;
    unsigned char OS_POL:1;
    unsigned char OS_COMP_INT:1;
    unsigned char SHUTDOWN:1;

} TMP75CConfReg;
typedef union
{
    TMP75CConfReg ConfigBit;
    unsigned char uc_Config;
} un_TMP75C_ConfigReg;

typedef enum
{
    OS_COMPARATOR_TMP75C = 0,
    OS_INTERRUPT_TMP75C = 1,
} TMP75C_OSMode;
typedef enum
{
    OS_ACTIVE_LOW_TMP75C = 0,
    OS_ACTIVE_HIGH_TMP75C = 1,
} TMP75C_OSPolarity;
typedef enum
{
    POWER_NORMAL_TMP75C  = 0,
    POWER_SHUTDOWN_TMP75C = 1,
} TMP75C_PowerMode;

enum TMP75CREG
{
    TEMP_REG_POINT_TMP75C = 0x00,
    CONFIG_REG_POINT_TMP75C = 0x01,
    Tlow_REG_POINT_TMP75C = 0x02,
    Thigh_REG_POINT_TMP75C  = 0x03,
    ONESHOT_REG_POINT_TMP75C = 0x04,
};

class TMP75C
{
public:
    static float ReadTempf(unsigned char adr);
    //    static bool ReadTempReq(unsigned char adr,float& fTemp);  //∂¡Œ¬∂»«Î«Û
    //    static bool SetThighTemp(unsigned char adr,float TempThigh);
    //    static bool SetTlowTemp(unsigned char adr,float TempTlow);
    //    static float ReadTlowTemp(unsigned char adr);
    //    static float ReadThighTemp(unsigned char adr);

// 	static unsigned char ReadConfig(unsigned char adr);
// 	static bool WriteConfig(unsigned char adr,un_TMP75C_ConfigReg ConfigReg);
    //static bool WriteOneshot(unsigned char adr);
    //    static void setPowerMode(unsigned char adr,TMP75C_PowerMode mode);


    //    static TMP75C_OSMode getOsMode(unsigned char adr);
    //    static void setOsMode(unsigned char adr,TMP75C_OSMode mode);
    //    static TMP75C_OSPolarity getOsPolarity(unsigned char adr);
    //    static void setOsPolarity(unsigned char adr,TMP75C_OSPolarity polarity);

    static unsigned char bAssigned[8];
    static unsigned short HandleID[8];
};

#endif
