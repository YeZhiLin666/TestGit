/*****************************************************************************
// 程序文件      :ADC16bit_CS5560.cpp     主要用于模数转换,将为模拟量输入转化为16位数字量的硬件芯片CS5560驱动。
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#include "../../COMMON/includes.h"


//增加RST  RST24  P1.26



void Adc16bit_CS5560::Adc_Init(unsigned char Device_Number)
{


    if(Device_Number == 0)    //I
    {

    }
    else if(Device_Number == 1)    //V
    {

    }
}

//2个ADC一起conv
void Adc16bit_CS5560::CS5560_CONV(void)
{
    //触发CONV
    LPC_GPIO1->CLR |= ADC_CONV;    //拉低
    ARBUS_DELAY(5);//5
    LPC_GPIO1->SET |= ADC_CONV;  //拉高

}

//这里分别读取I V
//I ssp0    V ssp1    同时还要操作ad9253  ad9251控制增益才可读取
//还要控制ADC开关通道时序
unsigned long int  Adc16bit_CS5560::CS5560_Read(char chn,char IVtype)    //第二个参数预留 控制增益需要
{
    unsigned long int  temp = 0;    //返回的值
    unsigned char status = 0;
    unsigned long uw32_timeout;

    //等待READY
    //FIO2PIN  0为低   1为高
    unsigned short Tx[3],Rx[3];
    if(IVtype == IBT_ADC_TYPE_I)
    {
        uw32_timeout = 0;

        //这里暂时这么处理，以后改成trigger模式    comment out chen 20130107
        while(LPC_GPIO2->PIN & BUSY1)    //wait busy pin  变低
        {
            if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
            {
                return 0;
            }
        }

        //调用ssp接口来读取数据
        //unsigned short Tx[3],Rx[3];
        Tx[0] = 0;
        Tx[1] = 0;
        Tx[2] = 0;
        status = SSPIFunc::RxTx_Frames_1CS(Tx,3,2);    //读回来的数据在 spi的Rx缓冲区内    //发送数组  长度  通道
        if(status == 0)    //SSP出错
        {
            Adc_reset(0);
            return 0;
        }
        //FIO1SET |= DCS1;
        for(char i =0; i<3; i++)
        {
            Rx[i] = SSPIFunc::SSPIRxBuff[2][i];    //接收数据
        }
        //spi_2468::SSP_RxTx_Frames(unsigned short *arry,unsigned char len,unsigned char chn);

        //把Rx组成成为u32
        temp |= Rx[0]<<16;
        temp |= Rx[1]<<8;
        temp |= Rx[2];

        temp = BPToUP(temp);    //转换成偏移码
        return temp;
    }
    else if(IVtype == IBT_ADC_TYPE_V)
    {
        uw32_timeout = 0;

        //这里暂时这么处理，以后改成trigger模式    comment out chen 20130107
        while(LPC_GPIO2->PIN & BUSY2)    //wait busy pin  变低
        {
            if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
            {
                return 0;
            }
        }

        //调用ssp接口来读取数据
        Tx[0] = 0;
        Tx[1] = 0;
        Tx[2] = 0;

        //FIO1CLR |= DCS1;    //触发CS
        status = SSPIFunc::RxTx_Frames_1CS(Tx,3,3);    //读回来的数据在 spi的Rx缓冲区内    //发送数组  长度  通道
        if(status == 0)    //SSP出错
        {
            Adc_reset(0);
            return 0;
        }
        //FIO1SET |= DCS1;
        for(char i =0; i<3; i++)
        {
            Rx[i] = SSPIFunc::SSPIRxBuff[3][i];    //接收数据
        }

        //把Rx组成成为u32
        temp |= Rx[0]<<16;
        temp |= Rx[1]<<8;
        temp |= Rx[2];

        temp = BPToUP(temp);    //转换成偏移码
        return temp;
    }
    return 0;
}
//如果BP/UP为高，则是双极
//输出为二补码  所以输出接近FFFFFF是 -mv的意思 基本正确
//正数的补码  相同
//负数的补码  取反+1



void Adc16bit_CS5560::Adc_reset(unsigned char Device_Number)
{
    //拉低RST
    LPC_GPIO1->CLR |= ADC_CS5560_RST;
    TIMERSFunc::DELAY_SYS(200);
    LPC_GPIO1->SET |= ADC_CS5560_RST;
}

//二补码转偏移码
unsigned long int Adc16bit_CS5560::BPToUP(unsigned long int temp)
{
    unsigned long int a = 0;
    if(temp & 0x800000)
    {
        a = temp - 0x800000;    //负向变换成线性下半段
    }
    else
    {
        a = temp + 0x800000;    //正向
    }

    return a;
}

