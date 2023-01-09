/*****************************************************************************
// �����ļ�      :ADC16bit_CS5560.cpp     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ16λ��������Ӳ��оƬCS5560������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#include "../../COMMON/includes.h"


//����RST  RST24  P1.26



void Adc16bit_CS5560::Adc_Init(unsigned char Device_Number)
{


    if(Device_Number == 0)    //I
    {

    }
    else if(Device_Number == 1)    //V
    {

    }
}

//2��ADCһ��conv
void Adc16bit_CS5560::CS5560_CONV(void)
{
    //����CONV
    LPC_GPIO1->CLR |= ADC_CONV;    //����
    ARBUS_DELAY(5);//5
    LPC_GPIO1->SET |= ADC_CONV;  //����

}

//����ֱ��ȡI V
//I ssp0    V ssp1    ͬʱ��Ҫ����ad9253  ad9251��������ſɶ�ȡ
//��Ҫ����ADC����ͨ��ʱ��
unsigned long int  Adc16bit_CS5560::CS5560_Read(char chn,char IVtype)    //�ڶ�������Ԥ�� ����������Ҫ
{
    unsigned long int  temp = 0;    //���ص�ֵ
    unsigned char status = 0;
    unsigned long uw32_timeout;

    //�ȴ�READY
    //FIO2PIN  0Ϊ��   1Ϊ��
    unsigned short Tx[3],Rx[3];
    if(IVtype == IBT_ADC_TYPE_I)
    {
        uw32_timeout = 0;

        //������ʱ��ô�����Ժ�ĳ�triggerģʽ    comment out chen 20130107
        while(LPC_GPIO2->PIN & BUSY1)    //wait busy pin  ���
        {
            if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
            {
                return 0;
            }
        }

        //����ssp�ӿ�����ȡ����
        //unsigned short Tx[3],Rx[3];
        Tx[0] = 0;
        Tx[1] = 0;
        Tx[2] = 0;
        status = SSPIFunc::RxTx_Frames_1CS(Tx,3,2);    //�������������� spi��Rx��������    //��������  ����  ͨ��
        if(status == 0)    //SSP����
        {
            Adc_reset(0);
            return 0;
        }
        //FIO1SET |= DCS1;
        for(char i =0; i<3; i++)
        {
            Rx[i] = SSPIFunc::SSPIRxBuff[2][i];    //��������
        }
        //spi_2468::SSP_RxTx_Frames(unsigned short *arry,unsigned char len,unsigned char chn);

        //��Rx��ɳ�Ϊu32
        temp |= Rx[0]<<16;
        temp |= Rx[1]<<8;
        temp |= Rx[2];

        temp = BPToUP(temp);    //ת����ƫ����
        return temp;
    }
    else if(IVtype == IBT_ADC_TYPE_V)
    {
        uw32_timeout = 0;

        //������ʱ��ô�����Ժ�ĳ�triggerģʽ    comment out chen 20130107
        while(LPC_GPIO2->PIN & BUSY2)    //wait busy pin  ���
        {
            if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
            {
                return 0;
            }
        }

        //����ssp�ӿ�����ȡ����
        Tx[0] = 0;
        Tx[1] = 0;
        Tx[2] = 0;

        //FIO1CLR |= DCS1;    //����CS
        status = SSPIFunc::RxTx_Frames_1CS(Tx,3,3);    //�������������� spi��Rx��������    //��������  ����  ͨ��
        if(status == 0)    //SSP����
        {
            Adc_reset(0);
            return 0;
        }
        //FIO1SET |= DCS1;
        for(char i =0; i<3; i++)
        {
            Rx[i] = SSPIFunc::SSPIRxBuff[3][i];    //��������
        }

        //��Rx��ɳ�Ϊu32
        temp |= Rx[0]<<16;
        temp |= Rx[1]<<8;
        temp |= Rx[2];

        temp = BPToUP(temp);    //ת����ƫ����
        return temp;
    }
    return 0;
}
//���BP/UPΪ�ߣ�����˫��
//���Ϊ������  ��������ӽ�FFFFFF�� -mv����˼ ������ȷ
//�����Ĳ���  ��ͬ
//�����Ĳ���  ȡ��+1



void Adc16bit_CS5560::Adc_reset(unsigned char Device_Number)
{
    //����RST
    LPC_GPIO1->CLR |= ADC_CS5560_RST;
    TIMERSFunc::DELAY_SYS(200);
    LPC_GPIO1->SET |= ADC_CS5560_RST;
}

//������תƫ����
unsigned long int Adc16bit_CS5560::BPToUP(unsigned long int temp)
{
    unsigned long int a = 0;
    if(temp & 0x800000)
    {
        a = temp - 0x800000;    //����任�������°��
    }
    else
    {
        a = temp + 0x800000;    //����
    }

    return a;
}

