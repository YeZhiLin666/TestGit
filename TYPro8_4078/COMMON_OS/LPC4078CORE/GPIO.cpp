
#include "includes.h"

const unsigned char GPIOFunc::GPIO_MODValue[PORT_MAX][PIN_MAX][MOD_MAX] =
{
    {   //Port 0
        {CAN_RD1,      U3_TXD,       I2C1_SDA,       U0_TXD,    NA,        NA,       NA    },//P0.0
        {CAN_TD1,      U3_RXD,       I2C1_SCL,       U0_RXD,    NA,        NA,       NA    },//P0.1
        {U0_TXD,       U3_TXD,       NA,             NA,        NA,        NA,       NA    },//P0.2
        {U0_RXD,       U3_RXD,       NA,             NA,        NA,        NA,       NA    },//P0.3
        {I2S_RX_SCK,   CAN_RD2,      T2_CAP0,        NA,        CMP_ROSC,  NA,       LCD   },//P0.4
        {I2S_RX_WS,    CAN_TD2,      T2_CAP1,        NA,        CMP_RESET, NA,       LCD   },//P0.5
        {I2S_RX_SDA,   SSP1_SSEL,    T2_MAT0,        U1_RTS,    CMP_ROSC,  NA,       LCD   },//P0.6
        {I2S_TX_SCK,   SSP1_SCK,     T2_MAT1,        RTC_EV0,   CMP_VREF,  NA,       LCD   },//P0.7
        {I2S_TX_WS,    SSP1_MISO,    T2_MAT2,        RTC_EV1,   CMP1_IN,   NA,       LCD   },//P0.8
        {I2S_TX_SDA,   SSP1_MOSI,    T2_MAT3,        RTC_EV2,   CMP1_IN,   NA,       LCD   },//P0.9
        {U2_TXD,       I2C2_SDA,     T3_MAT0,        NA,        NA,        NA,       LCD   },//P0.10
        {U2_RXD,       I2C2_SCL,     T3_MAT1,        NA,        NA,        NA,       LCD   },//P0.11
        {USB_PPWR2,    SSP1_MISO,    ADC0_IN,        NA,        NA,        NA,       NA    },//P0.12
        {USB_UP_LED2,  SSP1_MOSI,    ADC0_IN,        NA,        NA,        NA,       NA    },//P0.13
        {USB_HSTEN2,   SSP1_SSEL,    USB_CONNECT2,   NA,        NA,        NA,       NA    },//P0.14
        {U1_TXD,       SSP0_SCK,     NA,             NA,        SPIFI_IO,  NA,       NA    },//P0.15
        {U1_RXD,       SSP0_SSEL,    NA,             NA,        SPIFI_IO,  NA,       NA    },//P0.16
        {U1_CTS,       SSP0_MISO,    NA,             NA,        SPIFI_IO,  NA,       NA    },//P0.17
        {U1_DCD,       SSP0_MOSI,    NA,             NA,        SPIFI_IO,  NA,       NA    },//P0.18
        {U1_DSR,       SD_CLK,       I2C1_SDA,       NA,        NA,        NA,       LCD   },//P0.19
        {U1_DTR,       SD_CMD,       I2C1_SCL,       NA,        NA,        NA,       LCD   },//P0.20
        {U1_RI,        SD_PWR,       U4_OE,          CAN_RD1,   U4_SCLK,   NA,       NA    },//P0.21
        {U1_RTS,       SD_DAT,       U4_TXD,         CAN_TD1,   SPIFI_CLK, NA,       NA    },//P0.22
        {ADC0_IN,      I2S_RX_SCK,   T3_CAP0,        NA,        NA,        NA,       NA    },//P0.23
        {ADC0_IN,      I2S_RX_WS,    T3_CAP1,        NA,        NA,        NA,       NA    },//P0.24
        {ADC0_IN,      I2S_RX_SDA,   U3_TXD,         NA,        NA,        NA,       NA    },//P0.25
        {ADC0_IN,      DAC_OUT,      U3_RXD,         NA,        NA,        NA,       NA    },//P0.26
        {I2C0_SDA,     USB_SDA1,     NA,             NA,        NA,        NA,       NA    },//P0.27
        {I2C0_SCL,     USB_SCL1,     NA,             NA,        NA,        NA,       NA    },//P0.28
        {USB_Dplus1,     EINT0,      NA,             NA,        NA,        NA,       NA    },//P0.29
        {USB_Dminus1,     EINT1,     NA,             NA,        NA,        NA,       NA    },//P0.30
        {USB_Dplus2,      NA,        NA,             NA,        NA,        NA,       NA    },//P0.31
    },
    {   //Port 1
        {ENET_TXD0,    NA,           T3_CAP1,        SSP2_SCK,  NA,        NA,       NA    },//P1.0
        {ENET_TXD1,    NA,           T3_MAT3,        SSP2_MOSI, NA,        NA,       NA    },//P1.1
        {ENET_TXD2,    SD_CLK,       PWM0,           NA,        NA,        NA,       NA    },//P1.2
        {ENET_TXD3,    SD_CMD,       PWM0,           NA,        NA,        NA,       NA    },//P1.3
        {ENET_TX_EN,   NA,           T3_MAT2,        SSP2_MISO, NA,        NA,       NA    },//P1.4
        {ENET_TX_ER,   SD_PWR,       PWM0,           NA,        CMP1_INNA, NA,       NA    },//P1.5
        {ENET_TX_CLK,  SD_DAT,       PWM0,           NA,        CMP0_IN,   NA,       NA    },//P1.6
        {ENET_COL,     SD_DAT,       PWM0,           NA,        CMP1_IN,   NA,       NA    },//P1.7
        {ENET_CRS,     NA,           T3_MAT1,        SSP2_SSEL, NA,        NA,       NA    },//P1.8
        {ENET_RXD0,    NA,           T3_MAT0,         NA,        NA,        NA,       NA    },//P1.9
        {ENET_RXD1,    NA,           T3_CAP0,        NA,        NA,        NA,       NA    },//P1.10
        {ENET_RXD2,    SD_DAT,       PWM0,           NA,        NA,        NA,       NA    },//P1.11
        {ENET_RXD3,    SD_DAT,       PWM0_CAP0,      NA,        CMP1_OUT,  NA,       NA    },//P1.12
        {ENET_RX_DV,   NA,           NA,             NA,        NA,        NA,       NA    },//P1.13
        {ENET_RX_ER,   NA,           T2_CAP0,        NA,        CMP0_IN,   NA,       NA    },//P1.14
        {ENET_RX_CLK,  NA,           I2C2_SDA,       NA,        NA,        NA,       NA    },//P1.15
        {ENET_MDC,     I2S_TX_MCLK,  NA,             NA,        CMP0_IN,   NA,       NA    },//P1.16
        {ENET_MDIO,    I2S_RX_MCLK,  NA,             NA,        CMP0_IN,   NA,       NA    },//P1.17
        {USB_UP_LED1,  PWM1,         T1_CAP0,        NA,        SSP1_MISO, NA,       NA    },//P1.18
        {USB_TX_E1,    USB_PPWR1,    T1_CAP1,        MC_0A,     SSP1_SCK,  U2_OE,    NA    },//P1.19
        {USB_TX_DP1,   PWM1,         QEI_PHA,        MC_FB0,    SSP0_SCK,  LCD,      LCD   },//P1.20
        {USB_TX_DM1,   PWM1,         SSP0_SSEL,      MC_ABORT,  NA,        LCD,      LCD   },//P1.21
        {USB_RCV1,     USB_PWRD1,    T1_MAT0,        MC_0B,     SSP1_MOSI, LCD,      LCD   },//P1.22
        {USB_RX_DP1,   PWM1,         QEI_PHB,        MC_FB1,    SSP0_MISO, LCD,      LCD   },//P1.23
        {USB_RX_DM1,   PWM1,         QEI_IDX,        MC_FB2,    SSP0_MOSI, LCD,      LCD   },//P1.24
        {USB_LS1,      USB_HSTEN1,   T1_MAT1,        MC_1A,     CLKOUT,    LCD,      LCD   },//P1.25
        {USB_SSPND1,   PWM1,         T0_CAP0,        MC_1B,     SSP1_SSEL, LCD,      LCD   },//P1.26
        {USB_INT1,     USB_OVRCR1,   T0_CAP1,        CLKOUT,    NA,        LCD,      LCD   },//P1.27
        {USB_SCL1,     PWM1_CAP0,    T0_MAT0,        MC_2A,     SSP0_SSEL, LCD,      LCD   },//P1.28
        {USB_SDA1,     PWM1_CAP1,    T0_MAT1,        MC_2B,     U4_TXD,    LCD,      LCD   },//P1.29
        {USB_PWRD2,    USB_VBUS,     ADC0_IN,        I2C0_SDA,  U3_OE,     NA,       NA    },//P1.30
        {USB_OVRCR2,   SSP1_SCK,     ADC0_IN,        I2C0_SCL,  NA,        NA,       NA    },//P1.31
    },
    {   //Port 2
        {PWM1,         U1_TXD,       NA,             NA,        NA,        NA,       LCD   },//2.0
        {PWM1,         U1_RXD,       NA,             NA,        NA,        NA,       LCD   },//2.1
        {PWM1,         U1_CTS,       T2_MAT3,        NA,        TRACEDATA, NA,       LCD   },//2.2
        {PWM1,         U1_DCD,       T2_MAT2,        NA,        TRACEDATA, NA,       LCD   },//2.3
        {PWM1,         U1_DSR,       T2_MAT1,        NA,        TRACEDATA, NA,       LCD   },//2.4
        {PWM1,         U1_DTR,       T2_MAT0,        NA,        TRACEDATA, NA,       LCD   },//2.5
        {PWM1_CAP0,    U1_RI,        T2_CAP0,        U2_OE,     TRACECLK,  LCD,      LCD   },//2.6
        {CAN_RD2,      U1_RTS,       NA,             NA,        SPIFI_CS,  LCD,      LCD   },//2.7
        {CAN_TD2,      U2_TXD,       U1_CTS,         ENET_MDC,  NA,        LCD,      LCD   },//2.8
        {USB_CONNECT1, U2_RXD,       U4_RXD,         ENET_MDIO, NA,        LCD,      LCD   },//2.9
        {EINT0,        NMI,          NA,             NA,        NA,        NA,       NA    },//2.10
        {EINT1,        SD_DAT,       I2S_TX_SCK,     NA,        NA,        NA,       LCD   },//2.11
        {EINT2,        SD_DAT,       I2S_TX_WS,      LCD,       LCD,       LCD,      LCD   },//2.12
        {EINT3,        SD_DAT,       I2S_TX_SDA,     NA,        LCD,       LCD,      LCD   },//2.13
        {SDRAM_ENABLE, I2C1_SDA,     T2_CAP0,        NA,        NA,        NA,       NA    },//2.14
        {SDRAM_ENABLE, I2C1_SCL,     T2_CAP1,        NA,        NA,        NA,       NA    },//2.15
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.16
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.17
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.18
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.19
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.20
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.21
        {SDRAM_ENABLE, SSP0_SCK,     T3_CAP0,        NA,        NA,        NA,       NA    },//2.22
        {SDRAM_ENABLE, SSP0_SSEL,    T3_CAP1,        NA,        NA,        NA,       NA    },//2.23
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.24
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.25
        {SDRAM_ENABLE, SSP0_MISO,    T3_MAT0,        NA,        NA,        NA,       NA    },//2.26
        {SDRAM_ENABLE, SSP0_MOSI,    T3_MAT1,        NA,        NA,        NA,       NA    },//2.27
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.28
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//2.29
        {SDRAM_ENABLE, I2C2_SDA,     T3_MAT2,        NA,        NA,        NA,       NA    },//2.30
        {SDRAM_ENABLE, I2C2_SCL,     T3_MAT3,        NA,        NA,        NA,       NA    },//2.31
    },
    {   //Port 3
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.0
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.1
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.2
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.3
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.4
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.5
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.6
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.7
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.8
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.9
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.10
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.11
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.12
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.13
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.14
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P3.15
        {SDRAM_ENABLE, PWM0,         U1_TXD,         NA,        NA,        NA,       NA    },//P3.16
        {SDRAM_ENABLE, PWM0,         U1_RXD,         NA,        NA,        NA,       NA    },//P3.17
        {SDRAM_ENABLE, PWM0,         U1_CTS,         NA,        NA,        NA,       NA    },//P3.18
        {SDRAM_ENABLE, PWM0,         U1_DCD,         NA,        NA,        NA,       NA    },//P3.19
        {SDRAM_ENABLE, PWM0,         U1_DSR,         NA,        NA,        NA,       NA    },//P3.20
        {SDRAM_ENABLE, PWM0,         U1_DTR,         NA,        NA,        NA,       NA    },//P3.21
        {SDRAM_ENABLE, PWM0_CAP0,    U1_RI,          NA,        NA,        NA,       NA    },//P3.22
        {SDRAM_ENABLE, PWM1_CAP0,    T0_CAP0,        NA,        NA,        NA,       NA    },//P3.23
        {SDRAM_ENABLE, PWM1,         T0_CAP1,        NA,        NA,        NA,       NA    },//P3.24
        {SDRAM_ENABLE, PWM1,         T0_MAT0,        NA,        NA,        NA,       NA    },//P3.25
        {SDRAM_ENABLE, PWM1,         T0_MAT1,        STCLK,     NA,        NA,       NA    },//P3.26
        {SDRAM_ENABLE, PWM1,         T1_CAP0,        NA,        NA,        NA,       NA    },//P3.27
        {SDRAM_ENABLE, PWM1,         T1_CAP1,        NA,        NA,        NA,       NA    },//P3.28
        {SDRAM_ENABLE, PWM1,         T1_MAT0,        NA,        NA,        NA,       NA    },//P3.29
        {SDRAM_ENABLE, U1_RTS,       T1_MAT1,        NA,        NA,        NA,       NA    },//P3.30
        {SDRAM_ENABLE, NA,           T1_MAT2,        NA,        NA,        NA,       NA    },//P3.31
    },
    {   //Port 4
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.0
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.1
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.2
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.3
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.4
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.5
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.6
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.7
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.8
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.9
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.10
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.11
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.12
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.13
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.14
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.15
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.16
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.17
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.18
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.19
        {SDRAM_ENABLE, I2C2_SDA,     SSP1_SCK,       NA,        NA,        NA,       NA    },//P4.20
        {SDRAM_ENABLE, I2C2_SCL,     SSP1_SSEL,      NA,        NA,        NA,       NA    },//P4.21
        {SDRAM_ENABLE, U2_TXD,       SSP1_MISO,      NA,        NA,        NA,       NA    },//P4.22
        {SDRAM_ENABLE, U2_RXD,       SSP1_MOSI,      NA,        NA,        NA,       NA    },//P4.23
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.24
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.25
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.26
        {SDRAM_ENABLE, NA,           NA,             NA,        NA,        NA,       NA    },//P4.27
        {SDRAM_ENABLE, U3_TXD,       T2_MAT0,        NA,        LCD,       LCD,      LCD   },//P4.28
        {SDRAM_ENABLE, U3_RXD,       T2_MAT1,        I2C2_SCL,  LCD,       LCD,      LCD   },//P4.29
        {SDRAM_ENABLE, NA,           NA,             NA,        CMP0_OUT,  NA,       NA    },//P4.30
        {CPLD_ENABLE,  NA,           NA,             NA,        NA,        NA,       NA    },//P4.31
    },
    {   //Port 5
        {SDRAM_ENABLE, SSP2_MOSI,    T2_MAT2,        NA,        NA,        NA,       NA    },//P5.0
        {SDRAM_ENABLE, SSP2_MISO,    T2_MAT3,        NA,        NA,        NA,       NA    },//P5.1
        {NA,           NA,           T3_MAT2,        NA,        I2C0_SDA,  NA,       NA    },//P5.2
        {NA,           NA,           NA,             U4_RXD,    I2C0_SCL,  NA,       NA    },//P5.3
        {U0_OE,        NA,           T3_MAT3,        U4_TXD,    NA,        NA,       NA    },//P5.4
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//P5.5
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//6
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//7
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//8
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//9
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//10
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//11
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//12
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//13
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//14
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//15
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//16
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//17
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//18
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//19
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//20
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//21
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//22
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//23
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//24
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//25
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//26
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//27
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//28
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//29
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//30
        {NA,           NA,           NA,             NA,        NA,        NA,       NA    },//31
    },
};

#ifdef _DEBUG
unsigned char  GPIOFunc::GPIO_CheckResult[PORT_MAX][PIN_MAX];
//unsigned char  GPIOFunc::GPIO_MODValueError = 0;    //覣R碙衧D错误E齦?
#endif


//GPIO?U作 setGPort 蒫諥高l蚻鏑?    clrGPortl蚻鏑?
void GPIOFunc::Set_G_Port(unsigned char Port,unsigned char Pin)
{
    LPC_GPIO_TypeDef * this_GPIO;

    this_GPIO = ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0X20*Port));
    this_GPIO->SET = (0x01 << Pin);  // this_GPIO->SET |= (0x01 << Pin);  // Solve that GPIO Ctrl error
}
void GPIOFunc::Clr_G_Port(unsigned char Port,unsigned char Pin)
{
    LPC_GPIO_TypeDef * this_GPIO;

    this_GPIO = ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0X20*Port));
    this_GPIO->CLR = (0x01 << Pin);  // this_GPIO->CLR |= (0x01 << Pin); // Solve that GPIO Ctrl error
}

unsigned char GPIOFunc::RD_G_Port(unsigned char Port,unsigned char Pin)
{
    unsigned long uw32_temp = 0;
    LPC_GPIO_TypeDef * this_GPIO;

    this_GPIO = ((LPC_GPIO_TypeDef *) (LPC_AHB_BASE  + 0x18000 + 0X20*Port));
    uw32_temp = this_GPIO->PIN;
    uw32_temp >>= Pin;

    return (0x01 & uw32_temp);
}


void GPIOFunc::InitPortFunc(uint8 PortIdx,uint8 PinIdx,uint8 Func, uint8 Value)
{
    // 		LPC_GPIO_TypeDef * this_GPIO;

    if( ( PortIdx>6 || PinIdx>32 || Func>USB_CONNECT1 ) ||( PortIdx >= 5 && PinIdx>4) )
        return;

    switch(Func)
    {
    case GPIO_I:
    case GPIO_O:
    case GPIO_IO:
        LPC_GPIO_TypeDef * this_GPIO;
        //O釸dNn前四蝨szE莂鞥濬unctionLZ有CaD軱Z暂覣DRrz?
        (*(Arbin_IOCON_W *)(Arbin_IOCON_BASE + PortIdx*0X80 + PinIdx*0X04)).BitReg.FUNC = 0;    //O釸d?tNS謅?有?x80,0x04,最sA改蜸sizeof(struct);
        (*(Arbin_IOCON_W *)(Arbin_IOCON_BASE + PortIdx*0X80 + PinIdx*0X04)).BitReg.OD = 0;	// modified by Philip 3/31/2015
        this_GPIO = ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0X20*PortIdx));
        if(Func == GPIO_O)
        {
            this_GPIO->DIR |= (0x01 << PinIdx); 		// set corresponding bit
            if(Value)
                this_GPIO->SET = (0x01 << PinIdx);
            else
                this_GPIO->CLR = (0x01 << PinIdx);
        }
        else
            this_GPIO->DIR &=  ~(0x01 << PinIdx);
        break;
    case NA:
        break;
    default:
        unsigned char FuncIdx = 0;
        for(FuncIdx =0; FuncIdx<=MOD_MAX; FuncIdx++)
        {
            if( GPIO_MODValue[PortIdx][PinIdx][FuncIdx]== Func)
            {
                (*(Arbin_IOCON_W *)(Arbin_IOCON_BASE + PortIdx*0X80 + PinIdx*0X04)).BitReg.FUNC = FuncIdx+1;
                break;
            }
        }
        break;
    } //end of switch
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/

