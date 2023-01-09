
#ifndef _GPIO_4078_
#define _GPIO_4078_

#include "../Data_Manipulation/Datatypes.h"

////user definition////
//需要使用的模块按顺序设置为  1-255
//不使用的模块置为0

#define UART_ENABLE    0
#define CAN_ENABLE 	   0
#define I2S_ENABLE	   0
#define TIMER_ENABLE   0  //包括CAP和MAT    基本用于外部捕获
#define AD_ENABLE	     0
#define DA_ENABLE	     0
#define USB_ENABLE	   0
#define SD_ENABLE	     0
#define PWM_ENABLE	   0
#define EXTIN_ENABLE   0
#define QEI_ENABLE     0
#define NMI_ENABLE     0

// used in this application.

//#define SDRAM_ENABLE	1
#define SPI_ENABLE		2	 // SPIoíSSP2e3éá?á???2?·?
#define SSP_ENABLE		3
#define I2C_ENABLE	    4	 // commnet out chen
//#define CPLD_ENABLE     5
#define ENET_ENABLE     6


////end user definition////


//comment out chen
#define PM_IN		1
#define PM_OUT		2
#define PM_MOD		3

#define PORT_MAX    6     // ???ú×?′ó?μ P0 --- P5 // 
#define PIN_MAX     32    // 1ü??×?′ó?μ  PX.00 --- PX.31 // 
#define MOD_MAX     7     // ?￡ê?×?′ó?μ  I2S_ENABLE,CAN_ENABLE,TIMER_ENABLE,UART_ENABLE,and so on // 
#define PIN_ALL_TOTAL    165   // 1ü??è?2?×üêy P0.00 --- P5.04 Address  4*32+5 // 


//define  control  PIOX //
#define   PIO0    0        //control  P0 out 
#define   PIO1    1        //control  P1 out 
#define   PIO2    2        //control  P2 out 
#define   PIO3    3        //control  P3 out 
#define   PIO4    4        //control  P4 out 
#define   PIOALL    6     //  ALL GPIO Sum

typedef enum
{
    GPIO_I = 0,   //???GPIO
    GPIO_O,       //???GPIO
    GPIO_IO,      //????GPIO
    CPLD_I,      //CPLD??
    CPLD_O,      //
    CPLD_IO,
    _3TO8_I,
    _3TO8_O,
    _3TO8_IO,
    NA = 10,

    CAN_RD1,
    CAN_TD1,
    CAN_RD2,
    CAN_TD2,

    CMP_RESET,
    CMP_ROSC,
    CMP_VREF,
    CMP1_INNA,
    CMP1_IN,
    CMP1_OUT,
    CMP0_IN,
    CMP0_OUT,

    RTC_EV0,
    RTC_EV1,
    RTC_EV2,

    SPIFI_IO,
    SPIFI_CLK,
    SPIFI_CS,

    U0_TXD,
    U0_RXD,
    U0_OE,
    U1_RXD,
    U1_CTS,
    U1_DCD,
    U1_RTS,
    U1_DSR,
    U1_DTR,
    U1_RI,
    U1_TXD,
    U2_OE,
    U2_TXD,
    U2_RXD,
    U3_RXD,
    U3_OE,
    U3_TXD,
    U4_OE,
    U4_SCLK,
    U4_TXD,
    U4_RXD,

    DAC_OUT,
    ADC0_IN,

    I2C0_SCL,
    I2C2_SDA,
    I2C1_SDA,
    I2C1_SCL,
    I2C2_SCL,
    I2C0_SDA,

    EINT0,
    EINT1,
    EINT2,
    EINT3,

    T0_CAP0,
    T0_CAP1,
    T0_MAT0,
    T0_MAT1,
    T1_MAT1,
    T1_MAT2,
    T1_MAT0,
    T1_CAP0,
    T1_CAP1,
    T2_MAT0,
    T2_MAT1,
    T2_MAT2,
    T2_MAT3,
    T2_CAP0,
    T2_CAP1,
    T3_MAT3,
    T3_MAT1,
    T3_MAT2,
    T3_CAP0,
    T3_CAP1,
    T3_MAT0,

    SD_CLK,
    SD_CMD,
    SD_PWR,
    SD_DAT,

    PWM0,
    PWM1_CAP1,
    PWM1,
    PWM0_CAP0,
    PWM1_CAP0,

    ENET_MDC,
    ENET_MDIO,
    ENET_TXD0,
    ENET_TXD1,
    ENET_RXD0,
    ENET_RXD1,
    ENET_RXD2,
    ENET_TXD2,
    ENET_RXD3,
    ENET_TXD3,
    ENET_RX_DV,
    ENET_TX_ER,
    ENET_RX_ER,
    ENET_RX_CLK,
    ENET_CRS,
    ENET_COL,
    ENET_TX_CLK,
    ENET_TX_EN,

    I2S_TX_MCLK,
    I2S_RX_MCLK,
    I2S_RX_SCK,
    I2S_RX_WS,
    I2S_RX_SDA,
    I2S_TX_SDA,
    I2S_TX_SCK,
    I2S_TX_WS,

    CLKOUT,
    TRACEDATA,
    TRACECLK,
    NMI,
    SDRAM_ENABLE,
    CPLD_ENABLE,
    STCLK,
    LCD,

    QEI_PHA,
    QEI_PHB,
    QEI_IDX,

    SSP0_MOSI,
    SSP0_SSEL,
    SSP0_SCK,
    SSP0_MISO,
    SSP1_MISO,
    SSP1_MOSI,
    SSP1_SCK,
    SSP1_SSEL,
    SSP2_SSEL,
    SSP2_SCK,
    SSP2_MOSI,
    SSP2_MISO,

    MC_FB0,
    MC_FB1,
    MC_FB2,
    MC_0A,
    MC_0B,
    MC_1A,
    MC_1B,
    MC_2A,
    MC_2B,
    MC_ABORT,

    USB_Dminus1,
    USB_Dplus1,
    USB_Dplus2,
    USB_TX_DM1,
    USB_RX_DP1,
    USB_RX_DM1,
    USB_LS1,
    USB_RCV1,
    USB_PWRD1,
    USB_TX_DP1,
    USB_TX_E1,
    USB_PPWR1,
    USB_UP_LED1,
    USB_PPWR2,
    USB_UP_LED2,
    USB_HSTEN2,
    USB_CONNECT2,
    USB_HSTEN1,
    USB_SSPND1,
    USB_OVRCR2,
    USB_INT1,
    USB_OVRCR1,
    USB_SCL1,
    USB_SDA1,
    USB_PWRD2,
    USB_VBUS,
    USB_CONNECT1,
} MCU4078_IO_FUNC_NAME;


class sdram
{
public:
    static unsigned char MyIOCount;
    static unsigned char MyIO[57][4];
};

class eint    //外部中断
{
public:
    static unsigned char MyIOCount;
    static unsigned char MyIO[2][4];
};

class ethernet
{
public:
    static unsigned char MyIOCount;
    static unsigned char MyIO[18][4];
};
class cpld
{
public:
    static unsigned char MyIOCount;
    static unsigned char MyIO[1][4];
};

//其他的基本是靠SPI来初始化





////////GPIO////////
class GPIOFunc
{
public:
#ifdef _DEBUG
    static unsigned char GPIO_CheckResult[PORT_MAX][PIN_MAX];
#endif
    static const unsigned char GPIO_MODValue[PORT_MAX][PIN_MAX][MOD_MAX];
    static unsigned char GPIO_MODValueError;

public:
    //static void SetModuleIO(unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName);
    //static void GPIO_Set_n_Check(void);
    //static void GPIO_Init(void);
    //static void InitPort(unsigned char uc_Port, unsigned char uc_Pin, unsigned char uc_Mode, unsigned char uc_Value,unsigned char uc_Base);//·?3￡??òa
    static void Set_G_Port(unsigned char Port,unsigned char Pin);
    static void Clr_G_Port(unsigned char Port,unsigned char Pin);
    static unsigned char RD_G_Port(unsigned char Port,unsigned char Pin);
    static void InitPortFunc(uint8 PortIdx,uint8 PinIdx,uint8 Func, uint8 Value);

};

#endif

//执行过程
//执行GPIO_Set_n_Check()
//Set the settings of GPIO configs for all "hardware modules"
//然后debug是否有重复定义
//GPIO_Init()，它会调用InitPort按照表依次初始化


